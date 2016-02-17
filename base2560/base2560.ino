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
    A   0=null, 100=off, 101=on   ALARM mode:
                                      true  SMS:ALL   SIREN:ON   BEEPER:OFF
                                      false SMS:NONE  SIREN:OFF  BEEPER:V,T,H,W,G,C

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/
// [A0, A1, A2] [2,3,4 BTNs] [5 BUZZER mosfet] [6,7 LEDs] [8 SIREN mosfet] [10,11,12,13 SD_softSPI] [20,21 RTC_i2c] [38 TFT_LED] [36,40,42,44,46,48 TFT_softSPI] [49,50,51,52,53 NRF_hwSPI]

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include <Wire.h>
#include "DS3231.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <avr/wdt.h>
#include <Eeprom24C32_64.h>
#include <SdFat.h>
#include <SdFatUtil.h>

//SD_SPI_CONFIGURATION >= 3  // Must be set in SdFat/SdFatConfig.h
SdFatSoftSpi<10, 12, 11> SD_card; //MISO, MOSI, SCK
SdFile SD_file;
const uint8_t SD_CS = 13;
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
int16_t NRF_messageFromSensor[8] = {
  0, //V
  0, //T
  0, //H
  0, //W
  0, //G
  0, //M
  0, //C
  0  //A
};

//время последнего сигнала от сенсоров, если давно было => сенсор сломался или выключен
uint32_t millisPrevSignal_sensors[6] = {1, 1, 1, 1, 1, 1}; // pipe 0..5

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

#define TFT_CS 48
#define TFT_DC 44
#define TFT_MOSI 42
#define TFT_CLK 40
#define TFT_RST 46
#define TFT_MISO 36
Adafruit_ILI9341 myDisplay = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
uint8_t TFT_pinLedPower = 38;

//RTC I2C: 20 SDA, 21 SCL
RTClib RTC3231;
DS3231 SYS_DS3231;

//eeprom
static Eeprom24C32_64 eeprom24C32(0x50);
const uint8_t eeprom24C32_address_alarmMode = 0;

uint32_t BASE_sensorSilenceFaultMillis = 300000; //сенсор молчит более millis => он сломался
bool BASE_sensorIsOn[6] = {false, false, false, false, false, false}; //0 1..5

int16_t BASE_sensorDecodedParams[6][7] = {
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0}
};  //encoded params; 0==null;  [sensorNum][paramNum]

bool BASE_sensorParamsIsDanger[6][7] = {
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,}
}; //[sensorPipeNum][paramNum]


bool BASE_sensorParamsIsAvailable[6][7] = {
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,},
  {false, false, false, false, false, false, false,}
}; //[sensorPipeNum][paramNum]

//STATEMACHINE
uint32_t STATEMACHINE_prevMillis_300ms = 1L;
uint32_t STATEMACHINE_prevMillis_1s = 1L;
uint32_t STATEMACHINE_prevMillis_3s = 1L;
uint32_t STATEMACHINE_prevMillis_5s = 1L;
uint32_t STATEMACHINE_prevMillis_17s = 1L;
uint32_t STATEMACHINE_prevMillis_61s = 1L;
uint32_t STATEMACHINE_prevMillis_103s = 1L;

//GSM
String GSM_phoneNums[] = {};
uint8_t GSM_phoneNums_count = 0;
uint32_t GSM_periodParamAllowSMSMillis[7] = {   //millis between SMS //unsigned long 2^32-1
  24 * 3600000L, //voltage on sensor battery, V
  1 * 3600000L, //temperature, C
  1 * 3600000L, //humidity, %
  1 * 3600000L, //water leak, bool
  1 * 3600000L, //gas CH4, ADC value
  1 * 3600000L, //motion detector, bool
  1 * 3600000L //gas CO, ADC value
};
uint32_t GSM_paramPrevSMSMillis[7] = {1L, 1L, 1L, 1L, 1L, 1L, 1L};

String GSM_answerCLIP = "";
String GSM_answerCSQ = "";
String GSM_answerCPAS = "";
String GSM_answerCOPS = "";

const uint8_t GSM_queueLoop_size = 12;
String GSM_queueLoop_phones[GSM_queueLoop_size] = {""}; //0..11
String GSM_queueLoop_messages[GSM_queueLoop_size] = {""};
uint8_t GSM_queueLoop_pos = 0;
uint8_t GSM_queueLoop_stateMachine_pos = 0;
uint32_t GSM_prevPingSuccessAnswerMillis = 1; //send AT+CSQ, not get answer => RST GSM
uint8_t GSM_reset_pin = 23;

bool BASE_ALARM_MOTION_MODE = false;

//peripheral
bool BASE_buzzer_isNeed = false;
bool BASE_siren_isNeed = false;
uint8_t BASE_buzzer_pin = 5; //N-mosfet
uint8_t BASE_siren_pin = 8;  //N-mosfet

uint8_t BASE_voltage_base_pin = A0; //TODO ADC AREF set to inner 2.56V and make -R-R- voltage divider
uint8_t BASE_voltage_acdc_pin = A1; //TODO ADC AREF set to inner 2.56V and make -R-R- voltage divider
uint8_t BASE_voltage_battery_pin = A2; //TODO ADC AREF set to inner 2.56V and make -R-R- voltage divider

//menu
int8_t MENU_state = 1;
uint8_t MENU_btnPrev_pin = 2;
uint8_t MENU_btnNext_pin = 3;

#define DEBUG 1;

#define debugSerial Serial
#define gsmSerial Serial1

void setup() {
  MCUSR = 0;  //VERY VERY IMPORTANT!!!! ELSE WDT DOESNOT RESET, DOESNOT DISABLED!!!
  wdt_disable();

  pinMode(GSM_reset_pin, OUTPUT);
  digitalWrite(GSM_reset_pin, 1); //HIGH=normal, LOW=resetGSM

  pinMode(MENU_btnPrev_pin, INPUT);
  pinMode(MENU_btnNext_pin, INPUT);

  delay(2000); //for calming current & voltage fluctuations

#ifdef DEBUG
  debugSerial.begin(57600); //DBG
  delay(50);
#endif

  eeprom24C32.initialize();

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
  digitalWrite(SD_CS, 1); //sd off until init
  SD_init();
  delay(50);

  RTC_init();
  delay(50);
  RTC_setTimeFromSD();
  delay(50);
  //RTC_setTime();

  GSM_initPhoneNums();

  ALARM_restore_alarmModeState();

  ALARM_beepShort_buzzer();
  ALARM_beepShort_siren();

  wdt_enable(WDTO_8S); //if WDT not reset on 8s => atmega restarts
}

void loop() {
  wr();
  NRF_listen();
  wr();
  STATEMACHINE_loop();
  wr();
}
