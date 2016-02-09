#include <SdFat.h>
#include <SPI.h>
SdFat sd;
#define SD_ChipSelectPin 4 //+11,12,13
SdFile SD_file;
bool SD_isEnable = false;

#include <TMRpcm.h>
//pcmConfig.h__DISABLE_SPEAKER2    Disables default second speaker pin for compatibility with other libs. (pin 10 on Uno)
//pcmConfig.h__SDFAT               SdFat library uses less prog space and memory. See SDFAT example included with lib.
TMRpcm tmrpcm;

#include <Wire.h>
#include "DS3231.h"
RTClib RTC3231;  //A4 SDA,  A5 SCL
DS3231 SYS_DS3231;

uint8_t AN_alarm_hh = 0;
uint8_t AN_alarm_ii = 0;
//bool AN_isAllowAlarm = true; //disabled by curr play OR by button
//uint32_t AN_disallowAlarmMillis = 1;
//uint8_t AN_btn_mute = 5;

//uint32_t STATEMACHINE_prevMillis_250ms = 1;
uint32_t STATEMACHINE_prevMillis_5s = 1;

void setup() { 

  //pinMode(AN_btn_mute, INPUT);

  tmrpcm.speakerPin = 9; //9 on 328
  tmrpcm.setVolume(5);
  tmrpcm.quality(0);//Set '1' for 2x oversampling

  SD_init();
  delay(50);
  SD_makeHowtoFile();
  delay(50);  

  Wire.begin();
  delay(50);

  RTC_init();
  RTC_setTimeFromSD(); //dt.txt    yy.mm.dd.hh.ii.ss.dow.
  delay(50);
  setAlarmTimeFromSD(); //alarm.txt    hh.ii.
  delay(50);

  tmrpcm.play("alarm.wav");
}

void loop() {
  STATEMACHINE_loop();
}


