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

/*
  WARNING: A given file must not be opened by more than one FatFile object
     or file corruption may occur.

     note Directory files must be opened read only.  Write and truncation is
     not allowed for directory files.
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

----------читается
да-----------------------нет
берем дату из файла------создаем
закрыли

открыли|создали|обрезали до 0 (truncate)
пишем текущую дату
закрыли

  
  if (!SD_confFileGSM.open(filename, O_READ)) {
    if (!SD_confFileGSM.open(filename, O_WRITE | O_CREAT | O_TRUNC )) {
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
