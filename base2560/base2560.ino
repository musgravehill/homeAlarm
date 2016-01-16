//TODO
// 0. CALL => save phone to SD

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
    V   0=null, 100*V.xx  voltage on sensor battery, 100*V
    T   0=null, -50..120 [+100]   temperature, C
    H   0=null, 0..100   [+100]   humidity, %
    W   0=null, 0=null, (0,1 + 100) = 100=normal, 101=alert         water leak, bool
    G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    M   0=null, (0,1 + 100) = 100=normal, 101=alert motion detector, bool
    C   0=null, 0..1023 [+1]      gas CO, ADC value

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/
// [A0 voltage 1.1V] [2,3 ENCODER] [10,11,12,13 SD_softSPI] [20,21 RTC_i2c] [42 TFT_LED] [43,44,45,46,47,48 TFT_softSPI] [49,50,51,52,53 NRF_hwSPI]

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include <Wire.h>
#include "DS3231.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include <Encoder.h>
Encoder myEncoder(2, 3);

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
int16_t NRF_messageFromSensor[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

//время последнего сигнала от сенсоров, если давно было => сенсор сломался или выключен
uint64_t millisPrevSignal_sensors[6] = {1}; // pipe 0..5

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

#define TFT_CS 43
#define TFT_DC 44
#define TFT_MOSI 45
#define TFT_CLK 46
#define TFT_RST 47
#define TFT_MISO 48
Adafruit_ILI9341 myDisplay = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
uint8_t TFT_pinLedPower = 42;

//RTC I2C: 20 SDA, 21 SCL
RTClib RTC3231;
DS3231 SYS_DS3231;

uint16_t BASE_sensorSilenceFaultMillis = 300000; //сенсор молчит более 10s => он сломался
bool BASE_sensorIsOn[6] = {false}; //0 1..5
int16_t BASE_sensorDecodedParams[6][7] = {0}; //encoded params; 0==null;  [sensorNum][paramNum]
bool BASE_sensorParamsIsDanger[6][7] = {true}; //[sensorPipeNum][paramNum]
bool BASE_sensorParamsIsAvailable[6][7] = {true}; //[sensorPipeNum][paramNum]

//STATEMACHINE
uint32_t STATEMACHINE_prevMillis_1s = 1;
uint32_t STATEMACHINE_prevMillis_17s = 1;
uint32_t STATEMACHINE_prevMillis_61s = 1;
uint32_t STATEMACHINE_prevMillis_103s = 1;

//GSM
String GSM_phoneNums[] = {};
uint8_t GSM_phoneNums_count = 0;
uint32_t GSM_periodParamAllowSMSMillis[7] = {   //millis between SMS //unsigned long 2^32-1
  24 * 3600000, //voltage on sensor battery, V
  1 * 3600000, //temperature, C
  1 * 3600000, //humidity, %
  1 * 3600000, //water leak, bool
  1 * 3600000, //gas CH4, ADC value
  1 * 3600000, //motion detector, bool
  1 * 3600000 //gas CO, ADC value
};
uint32_t GSM_paramPrevSMSMillis[7] = {1};  //BUG: powerDown->powerUp->this vars will be skip to 0 => SMS_send is allow again
String GSM_answerCLIP = "";
String GSM_answerCSQ = "";
String GSM_answerCPAS = "";
String GSM_answerCOPS = "";

//peripheral
bool BASE_buzzerIsNeed = true;
uint8_t BASE_voltagePin = A0; //TODO ADC AREF set to inner 1.1V and make -R-R- voltage divider

//menu
int8_t MENU_state = 0;

#define DEBUG 1;

#define debugSerial Serial
#define gsmSerial Serial1

void setup() {
  delay(2000); //for calming current & voltage fluctuations

#ifdef DEBUG
  debugSerial.begin(57600); //DBG
  delay(50);
#endif

  gsmSerial.begin(57600, SERIAL_8N1); //default for Arduino & SIM800L
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

  //RTC_setTime();

  GSM_initPhoneNums();

  BASE_buzzerIsNeed = true;

#ifdef DEBUG
  debugSerial.print("GSM_phoneNums_count=");
  debugSerial.println(GSM_phoneNums_count, DEC);
  for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
    debugSerial.print(i, DEC);
    debugSerial.print("___");
    debugSerial.print(GSM_phoneNums[i]);
    debugSerial.println("___");
  }
#endif

}

void loop() {
  NRF_listen();
  ENCODER_read();
  STATEMACHINE_loop();
}

void BASE_processDataFromSensor() {
  millisPrevSignal_sensors[NRF_currPipeNum] =  millis(); //save time of sensor answer
  String string_logs = "LOGS;#" + String(NRF_currPipeNum, DEC) + ";";
  String string_dangers = "";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  int16_t paramVal_decoded;

  DateTime now = RTC3231.now();
  uint8_t hh =  now.hour();
  uint8_t ii =  now.minute();
  String hhii = ((hh < 10) ? "0" : "") + String(hh, DEC) + ":" ;
  hhii += ((ii < 10) ? "0" : "") + String(ii, DEC);

  for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
    if (NRF_messageFromSensor[paramNum] > 0) { //param is available
      paramVal_decoded = PARAMS_decodeParam(paramNum, NRF_messageFromSensor[paramNum]); //decode to real range
      string_logs +=  String((char)paramCode[paramNum]) + ";" + String(paramVal_decoded, DEC) + ";";
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = true;
      BASE_sensorDecodedParams[NRF_currPipeNum][paramNum] = paramVal_decoded;

      //param is danger
      if (PARAMS_isDangerParamValue(paramNum, paramVal_decoded)) {
        string_dangers = "DNGR;#" + String(NRF_currPipeNum, DEC) + ";";
        string_dangers += String((char)paramCode[paramNum]) + ";";
        string_dangers += String(paramVal_decoded, DEC) + ";" + hhii + ";";
        SD_log(string_dangers);
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = true;
      }
      else {
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = false;
      }
    }
    //param NOT available
    else {
      string_logs += String((char)paramCode[paramNum]) +  ";;";
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = false;
    }
  }

  string_logs += hhii + ";";
  SD_log(string_logs);
  GSM_sendDangers();

#ifdef DEBUG
  debugSerial.println(string_logs);
  debugSerial.println(string_dangers);
#endif
}

void BASE_checkSensorsFault() {
  uint32_t millisCurrSignal = millis();
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) { //SENSORS PIPES 1..5!
    int32_t deltaSignal = millisCurrSignal - millisPrevSignal_sensors[sensorPipeNum];
    if (deltaSignal >  BASE_sensorSilenceFaultMillis) {
      BASE_sensorIsOn[sensorPipeNum] = false; //sensor fault
    }
    else {
      BASE_sensorIsOn[sensorPipeNum] = true; //sensor ok
    }
  }
}


