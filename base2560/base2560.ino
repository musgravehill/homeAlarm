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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include <SdFat.h>
#include <stdint.h>
#include <Wire.h>
#include "DS3231.h"

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

//0=null  1=ok 2=danger
uint8_t BASE_isDangerParams[7] = {0};

//время последнего сигнала от сенсоров, если давно было => сенсор сломался или выключен
uint32_t millisPrevSignal_sensors[5] = {0};

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);


/*
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 6 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 4 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 2 - LCD reset (RST)
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(6, 5, 4, 3, 2);

const int LED_latchPin = A0; //ST_CP 74HC595
const int LED_clockPin = A1; //SH_CP 74HC595
const int LED_dataPin = A2; //DS 74HC595
uint16_t LED_twoBytes = 0b0;
*/

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

  //TFT_init();
  //delay(50);

  //LED_init();
  //delay(50);

  GSM_init();
  delay(50);
}

void loop() {
  NRF_listen();
  BASE_checkSensorsFault();
}

void BASE_processDataFromSensor() {
  String string_logs = "{LOGS;#" + String(NRF_currPipeNum, DEC) + ";";
  String string_dangers = "";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  uint16_t paramVal_decoded;

  for (uint8_t i = 0; i < 7; i++) {
    if (NRF_messageFromSensor[i] != 0) { //param is available
      paramVal_decoded = PARAMS_decodeParam(i, NRF_messageFromSensor[i]);
      string_logs +=  String((char)paramCode[i]) + String(paramVal_decoded, DEC) + ";";
      if (PARAMS_isDangerParamValue(i, paramVal_decoded)) {
        string_dangers += "{DNGR;#" + String(NRF_currPipeNum, DEC) + ";";
        string_dangers += String((char)paramCode[i]);
        string_dangers += String(paramVal_decoded, DEC) + ";}";

        BASE_isDangerParams[i] = 2; //0..6 params  0=null 1=ok 2=danger
      }
      else {
        if (BASE_isDangerParams[i] != 2) { //save DNGR forever, until you reboot
          BASE_isDangerParams[i] = 1; //ok
        }
      }
    }
    else {
      string_logs += String((char)paramCode[i]) +  "_;";
      BASE_isDangerParams[i] = 0; //null
    }
  }
  string_logs += "}";
  String commandToBaseSdGsmRtc_all = string_logs + string_dangers;


#ifdef DEBUG
  debugSerial.println(commandToBaseSdGsmRtc_all);
#endif

  //myDisplay.clearDisplay();
  //myDisplay.setTextSize(1);
  //myDisplay.setTextColor(BLACK);
  //myDisplay.setCursor(0, 0);
  //myDisplay.println(commandToBaseSdGsmRtc_all);
  //myDisplay.display();

  millisPrevSignal_sensors[NRF_currPipeNum] =  millis(); //save time of sensor answer

 // LED_paramsState();
}

void BASE_checkSensorsFault() {
  uint32_t millisCurrSignal = millis();
  //myDisplay.fillRect(0, 40, 84, 8, 0);//clear white stripe for icons
  for (int sensorNum = 1; sensorNum <= 5; sensorNum++) { //SENSORS PIPES 1..5!
    //myDisplay.fillRect(sensorNum * 12, 40, 8, 8, 1); //black icon == ok
    uint32_t deltaSignal = millisCurrSignal - millisPrevSignal_sensors[sensorNum];
    if (deltaSignal >  10000) { //10s
      //sensor fault
      //myDisplay.fillRect((sensorNum * 12 + 1), 41, 6, 6, 0); //white icon == fault
    }
    else {
      //sensor ok
    }
  }
  //myDisplay.display();
}


