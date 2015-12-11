/*
  NRF[encoded value in uint16_t] --> baseNRF --> decoded "command params"  --> baseSdGsmRtc

  command from baseNRF
    {LOGS;#2;V3.7;T23;H50}
    {DNGR;#5;W1}

  command params
    V   n, 0..5         voltage on sensor battery, V
    T   n, -50..120     temperature, C
    H   n, 0..100       humidity, %
    W   n, 0, 1         water leak, bool
    P   1               power up base, bool
    G   n, 0..1023 ADC  gas CH4, ADC value
    M   n, 0, 1         motion detector, bool
    C   n, 0..1023      gas CO, ADC value

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

SoftwareSerial sftSrl_forCommand(7, 8); // RX, TX
String command;

void setup() {
  delay(2000);
  pinMode(SD_CS, OUTPUT);
  pinMode(10, OUTPUT); //hardware ATmega SPI SS
  delay(50);
  Serial.begin(9600);
  delay(50);
  sftSrl_forCommand.begin(9600);
  delay(50);
  SD_init();
  delay(50);
  Wire.begin();
  delay(50);
}

void loop() {
  bool isEndOfCommand = false;
  char getByte;
  while (sftSrl_forCommand.available() & !isEndOfCommand ) {
    char getByte = sftSrl_forCommand.read();
    if (getByte == '{') { //new command start, clean buffer
      command = "";
    }
    if (getByte == '}') { //end of command
      isEndOfCommand = true;
      processCommand(command);
      command = "";
    }
    else {
      command += getByte;
    }
  }

  //----------------TEST-----------------
  processCommand("LOGS;#2;V3.7;T23;H50");
  processCommand("DNGR;#5;W1");
  processCommand("DNGR;#1;H90");
  delay(8000);
  //-------TEST END----------------------
}

//LOGS;#2;V3.7;T23;H50
//DNGR;#5;W1
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
  String message = "";
  message += String(hh, DEC) + ":" + String(ii, DEC) + " ";
  message += String(dd, DEC) + "-" + String(mm, DEC) + "-" + String(yyyy, DEC);
  message += ";" + command;
  

  if (commandType == "DNGR") { //LOGS, ALRT
    COMMAND_getVerbalParamName(commandParam0);
    GSM_sendSMS(message, "+79998885533");
  }
  SD_log(message);

  Serial.print(F("processCommand: "));
  Serial.println(command);
  Serial.print(F("commandParam0: "));
  Serial.println(commandParam0);
  Serial.print(F("message: "));
  Serial.println(message);
  Serial.print(F("\r\n"));
}

void SD_log(String data) {
  if (SD_isEnable) {
    // 8.3 filename.ext rule
    if (SD_file_log.open("log.txt", O_WRITE | O_CREAT | O_APPEND)) {
      SD_file_log.println(data);
      SD_file_log.close();
    }
  }
}

void SD_init() {
  if (SD_card.begin(SD_CS, SPI_HALF_SPEED)) {
    SD_isEnable = true;
    //Serial.println("SD init OK");
  } else {
    SD_isEnable = false;
    //Serial.println("SD init ERROR");
  }
}

void GSM_sendSMS(String message, String phone) {
  Serial.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  Serial.print(message);
  delay(300);
  Serial.print((char)26);
  delay(300);
}

String COMMAND_getVerbalParamName(String systemParamName) {
  if (systemParamName == "V") {
    return "_VOLTAGE_";
  }
  if (systemParamName == "T") {
    return "_TEMPERATURE_";
  }
  if (systemParamName == "H") {
    return "_HUMIDITY_";
  }
  if (systemParamName == "W") {
    return "_WATER_LEAK_";
  }
  if (systemParamName == "P") {
    return "_POWERUP_BASE_";
  }
  if (systemParamName == "G") {
    return "_GAS_CH4_";
  }
  if (systemParamName == "M") {
    return "_MOTION_DETECTION_";
  }
  if (systemParamName == "C") {
    return "_GAS_CO_";
  }
  return "CNNT_RECOGNIZE_PARAM";
}

/*
   String str;

  void loop()
  {
    if(Serial.available() > 0)
    {
        str = Serial.readStringUntil('\n');
        x = Serial.parseInt();
    }
  }
*/

