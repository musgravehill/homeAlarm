/*
  SD structure
  -log.txt     all logs here
  -phones.txt  phones for sms, call, etc

  TITLE            FILENAME     PERIOD_SECONDS
  CH4             ch4.sms
  humidity        hmdt.sms
  motionDetector  motdet.sms
  powerUp         powup.sms
  sensorVoltage   volt.sms      24*3600 [1 sms per 1 day]
  temperature     t.sms
  waterLeak       wleak.sms
*/

#include <SPI.h>
#include <SdFat.h>
SdFat SD_card;
SdFile SD_confFileGSM;
bool SD_isEnable = false;

uint32_t RTC_getUnixtime() {
  uint32_t u = 1279393393;
  return u;
}

bool conf_isAllowSMS(char* type) {
  //смс каждого типа ограничены промежутками. Например, вольты -1 раз в сутки.
  //в файле вольты.sms храним время предыдущей отправки
  char filename = type;
  uint32_t currUnixtime = RTC_getUnixtime();
  if (!SD_isEnable) {
    return false;
  }
  if (!SD_confFileGSM.open(filename, O_READ)) {
    if (!SD_confFileGSM.open(filename, O_WRITE | O_CREAT )) {
      return false;
    }
    else {
      myFile.println("");
    }
  }

}


void setup() {
  if (SD_card.begin(10, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void loop() {

}
