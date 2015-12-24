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

#include <SdFat.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "DS3231.h"

RTClib RTC3231;

const byte SD_CS =  10; //SS
SdFat SD_card;
SdFile SD_file_log;
bool SD_isEnable = false;

SoftwareSerial sftSrl_forGSM(7, 8); // RX, TX
String command;

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

void setup() {
  delay(2000);
  pinMode(SD_CS, OUTPUT);
  pinMode(10, OUTPUT); //hardware ATmega SPI SS
  delay(50);
  Serial.begin(9600);
  delay(50);
  sftSrl_forGSM.begin(9600);
  delay(50);
  SD_init();
  delay(50);
  Wire.begin();
  delay(50);

  sftSrl_forGSM.println("AT+CLIP=1"); //turn on caller ID notification
  delay(100);
  sftSrl_forGSM.println("AT+CMGF=1"); //Switching to text mode
  delay(100);
  sftSrl_forGSM.println("AT+CSCS=\"GSM\""); //english only
  delay(100);
}

void loop() {
  bool isEndOfCommand = false;
  char getByte;
  while (Serial.available() & !isEndOfCommand ) {
    char getByte = Serial.read();
    if (getByte == '}') { //end of command
      isEndOfCommand = true;
      processCommand(command);
      command = "";
    }
    else {
      command += getByte;
    }
    if (getByte == '{') { //new command start, clean buffer
      command = "";
    }
  }
}

//LOGS;#2;V3.7;T23;H50;
//DNGR;#5;W1;
void processCommand(String command) {
  /*
    DateTime now = RTC3231.now();
    uint32_t nowUt = now.unixtime();
    uint16_t yyyy =  now.year();
    uint8_t mm = now.month();
    uint8_t dd =  now.day();
    uint8_t hh = now.hour();
    uint8_t ii = now.minute();
  */

  uint32_t nowUt = 1449823614;
  uint16_t yyyy =  2015;
  uint8_t mm = 12;
  uint8_t dd = 28;
  uint8_t hh = 14;
  uint8_t ii = 29;

  String commandType = command.substring(0, 4);
  String commandParam0 = command.substring(8, 9);
  String messageLog = "";
  messageLog += String(hh, DEC) + ":" + String(ii, DEC) + " ";
  messageLog += String(dd, DEC) + "-" + String(mm, DEC) + "-" + String(yyyy, DEC);
  messageLog += ";" + command;
  SD_log(messageLog);

  if (commandType == "DNGR") { //LOGS, ALRT
    messageLog = COMMAND_getVerbalParamName(commandParam0) + messageLog;
    GSM_sendSMS(messageLog, "+79998885533");
  }

  //Serial.print(F("\r\n processCommand: "));
  //Serial.println(command);
  //Serial.print(F("\r\n"));
  //Serial.print(F("commandParam0: "));
  //Serial.println(commandParam0);
  //Serial.print(F("message: "));
  //Serial.println(messageLog);
  //Serial.print(F("\r\n"));
}

void SD_log(String data) {
  if (SD_isEnable) {
    /*
      DateTime now = RTC3231.now();
      uint32_t nowUt = now.unixtime();
      uint16_t yyyy =  now.year();
      uint8_t mm = now.month();
      uint8_t dd =  now.day();
    */

    uint16_t yy =  2015 - 2000;
    uint8_t mm = 12;
    uint8_t dd = 28;
    String filename_s = String(dd, DEC) + "." + String(mm, DEC) + "." + String(yy, DEC) + ".csv";
    char filename_chr[filename_s.length() + 1];
    filename_s.toCharArray(filename_chr, sizeof(filename_chr));

    // 8.3 filename.ext rule
    if (SD_file_log.open( filename_chr, O_WRITE | O_CREAT | O_APPEND)) {
      SD_file_log.println(data);
      SD_file_log.close();
    }
  }
}

void SD_init() {
  if (SD_card.begin(SD_CS, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void GSM_sendSMS(String message, String phone) {
  sftSrl_forGSM.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  sftSrl_forGSM.print(message);
  delay(300);
  sftSrl_forGSM.print((char)26);
  delay(300);

  //clean ALL SMS (in, out, read, unread, sent, unsent)
  sftSrl_forGSM.println("AT+CMGD=1,4");
  delay(300);
}

String COMMAND_getVerbalParamName(String systemParamName) {
  if (systemParamName == "V") {
    return F("_VOLTAGE_");
  }
  if (systemParamName == "T") {
    return F("_TEMPERATURE_");
  }
  if (systemParamName == "H") {
    return F("_HUMIDITY_");
  }
  if (systemParamName == "W") {
    return F("_WATER_LEAK_");
  }
  if (systemParamName == "G") {
    return F("_GAS_CH4_");
  }
  if (systemParamName == "M") {
    return F("_MOTION_DETECTION_");
  }
  if (systemParamName == "C") {
    return F("_GAS_CO_");
  }
  return F("CNNT_RECOGNIZE_PARAM");
}
