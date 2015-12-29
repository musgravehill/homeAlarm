/*
  base with NRF, GSM, SD, RTC, TFT
  MEGA_2560 3.6V 16MHz => no level-shifters
  3.6V for ALL devicies
  Get data from sensors 1..5 by NRF
*/
/*
  Последовательная шина: 0 (RX) и 1 (TX)
  Последовательная шина 1: 19 (RX) и 18 (TX)
  Последовательная шина 2: 17 (RX) и 16 (TX)
  Последовательная шина 3: 15 (RX) и 14 (TX)  .
  Внешнее прерывание:
    2 (прерывание 0)
    3 (прерывание 1)
    18 (прерывание 5)
    19 (прерывание 4)
    20 (прерывание 3)
    21 (прерывание 2)
  PWM: 2 до 13 и 44-46. Любой из выводов обеспечивает ШИМ с разрешением 8 бит при помощи функции analogWrite().
  SPI: 50 (MISO), 51 (MOSI), 52 (SCK), 53 (SS) или ICSP
  I2C: 20 (SDA) и 21 (SCL)
*/
/*
  NRF[encoded value in uint16_t] --> baseNRF --> decoded "command params"  --> baseSdGsmRtc

  command from baseNRF
    {LOGS;#1;V_;T21;H48;W_;G_;M_;C_;}
    {DNGR;#1;T21;}

  command params
    V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
    T   0=null, -50..120 [+100]   temperature, C
    H   0=null, 0..100   [+100]   humidity, %
    W   0=null, 0=null, (0,1 + 100) = 100=normal, 101=alert         water leak, bool
    G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    M   0=null, (0,1 + 100) = 100=normal, 101=alert motion detector, bool
    C   0=null, 0..1023 [+1]      gas CO, ADC value

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/
// [10,11,12,13 SD][20,21 RTC] [43,44,45,46,47,48 TFT] [49,50,51,52,53 NRF]

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include <Wire.h>
#include "DS3231.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include <SdFat.h>
#include <SdFatUtil.h>
//SD_SPI_CONFIGURATION >= 3  // Must be set in SdFat/SdFatConfig.h
SdFatSoftSpi<12, 11, 13> SD_card; //12 MISO, 11 MOSI, 13 SCK
SdFile SD_file;
const uint8_t SD_CS = 10;
bool SD_isEnable = false;

//NRF: 49 CE, 50 MISO, 51 MOSI, 52 SCK, 53 SS
#define NRF_CE_PIN 49 //custom nrf-s pin for listen\transmit\sleep signal
#define NRF_CSN_PIN 53 //hardware SS SPI

//'static' - no need
const uint64_t NRF_pipes[6] = {
  0xDEADBEEF00LL,  //pipe0 is SYSTEM_pipe, avoid openReadingPipe(0, );
  0xDEADBEEF01LL,
  0xDEADBEEF02LL,
  0xDEADBEEF03LL,
  0xDEADBEEF04LL,
  0xDEADBEEF05LL
};

uint8_t NRF_currPipeNum;
uint16_t NRF_messageFromSensor[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

//время последнего сигнала от сенсоров, если давно было => сенсор сломался или выключен
uint32_t millisPrevSignal_sensors[5] = {0};

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

#define TFT_CS 43
#define TFT_DC 44
#define TFT_MOSI 45
#define TFT_CLK 46
#define TFT_RST 47
#define TFT_MISO 48
Adafruit_ILI9341 myDisplay = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//RTC I2C: 20 SDA, 21 SCL
RTClib RTC3231;

//seconds between SMS
uint32_t periodAllowSMS_V = 24 * 3600; //voltage on sensor battery, V
uint32_t periodAllowSMS_T = 12 * 3600; //temperature, C
uint32_t periodAllowSMS_H = 12 * 3600; //humidity, %
uint32_t periodAllowSMS_W = 12 * 3600; //water leak, bool
uint32_t periodAllowSMS_G = 12 * 3600; //gas CH4, ADC value
uint32_t periodAllowSMS_M = 12 * 3600; //motion detector, bool
uint32_t periodAllowSMS_C = 12 * 3600; //gas CO, ADC value

//BUG: powerDown->powerUp->this vars will be skip to 0 => SMS_send is allow again
uint32_t unixtimePrevSMS_V = 0; //n, 0..5         voltage on sensor battery, V
uint32_t unixtimePrevSMS_T = 0; //n, -50..120     temperature, C
uint32_t unixtimePrevSMS_H = 0; //n, 0..100       humidity, %
uint32_t unixtimePrevSMS_W = 0; //n, 0, 1         water leak, bool
uint32_t unixtimePrevSMS_G = 0; //n, 0..1023 ADC  gas CH4, ADC value
uint32_t unixtimePrevSMS_M = 0; //n, 0, 1         motion detector, bool
uint32_t unixtimePrevSMS_C = 0; //n, 0..1023      gas CO, ADC value

bool BASE_sensorIsOk[6] = {false}; //0 1..5
uint16_t BASE_sensorParams[6][7] = {0}; //encoded uint params; 0==null;  [sensorNum][paramNum]
bool BASE_sensorParamsIsDanger[6][7] = {true}; //[sensorNum][paramNum]

unsigned long STATEMACHINE_prevMillis_5s;
unsigned long STATEMACHINE_prevMillis_10s;

#define DEBUG 1;

#define debugSerial Serial
#define gsmSerial Serial1

void setup() {
  delay(2000); //for calming current & voltage fluctuations

#ifdef DEBUG
  debugSerial.begin(9600); //DBG
  delay(50);
#endif

  gsmSerial.begin(9600); //GSM
  delay(50);

  Wire.begin();
  delay(50);

  NRF_init();
  delay(50);

  TFT_init();
  delay(50);

  GSM_init();
  delay(50);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH); //disable SD
  SD_init();
  delay(50);

  RTC_setTime();
}

void loop() {
  NRF_listen();
  STATEMACHINE_loop();
}

void BASE_processDataFromSensor() {
  millisPrevSignal_sensors[NRF_currPipeNum] =  millis(); //save time of sensor answer
  String string_logs = "{LOGS;#" + String(NRF_currPipeNum, DEC) + ";";
  String string_dangers = "";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  int16_t paramVal_decoded; //int -+;  not uint

  for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
    BASE_sensorParams[NRF_currPipeNum][paramNum] = NRF_messageFromSensor[paramNum];//save encoded params for TFT

    //param is available
    if (NRF_messageFromSensor[paramNum] != 0) {
      paramVal_decoded = PARAMS_decodeParam(paramNum, NRF_messageFromSensor[paramNum]); //decode to real range
      string_logs +=  String((char)paramCode[paramNum]) + String(paramVal_decoded, DEC) + ";";

      //param is danger
      if (PARAMS_isDangerParamValue(paramNum, paramVal_decoded)) {
        string_dangers += "{DNGR;#" + String(NRF_currPipeNum, DEC) + ";";
        string_dangers += String((char)paramCode[paramNum]);
        string_dangers += String(paramVal_decoded, DEC) + ";}";
        SD_log(string_dangers);
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = true;
      }
      else {
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = false;
      }
    }
    //param NOT available
    else {
      string_logs += String((char)paramCode[paramNum]) +  "_;";
    }
  }
  string_logs += "}";
  SD_log(string_logs);

#ifdef DEBUG
  debugSerial.println(string_logs);
  debugSerial.println(string_dangers);  
#endif
}

void BASE_checkSensorsFault() {
  uint32_t millisCurrSignal = millis();
  for (int sensorNum = 1; sensorNum <= 5; sensorNum++) { //SENSORS PIPES 1..5!
    uint32_t deltaSignal = millisCurrSignal - millisPrevSignal_sensors[sensorNum];
    if (deltaSignal >  10000) { //10s
      //sensor fault
      BASE_sensorIsOk[sensorNum] = false;
    }


    else {
      //sensor ok
      BASE_sensorIsOk[sensorNum] = true;
    }
  }
}


