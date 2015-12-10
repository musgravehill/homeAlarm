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
SdFile SD_filePeriodSms;
bool SD_isEnable = false;

uint32_t RTC_getUnixtime() {
  uint32_t u = 1279393393;
  return u;
}

bool conf_isAllowSMS(String type) {
  //смс каждого типа ограничены промежутками. Например, вольты -1 раз в сутки.
  //в файле вольты.sms храним время предыдущей отправки
  char filename[sizeof(type)];
  type.toCharArray(filename, sizeof(type));

  uint32_t currUnixtime = RTC_getUnixtime();
  char* prevUnixtime_chr = {};
  uint32_t prevUnixtime = 0;
  bool res_SDisDamaged = false;
  bool res_isAllowByPeriod = false;

  uint16_t periodByTypeSms = 100;//seconds  TODO

  Serial.print("curr: ");
  Serial.print(currUnixtime, DEC);
  Serial.println(" ");

  if (!SD_isEnable) {
    Serial.println("!SD_isEnable");
    res_SDisDamaged = true;
  }
  /*
    ----------читается
    да-----------------------нет
    берем дату из файла------создаем
    закрыли

    открыли|создали|обрезали до 0 (truncate)
    пишем текущую дату
    закрыли
  */

  if (SD_filePeriodSms.open(filename, O_READ)) { //check period
    Serial.println("open_read");
    uint8_t i = 0;
    while (SD_filePeriodSms.available()) {
      prevUnixtime_chr[i] = SD_filePeriodSms.read();
      i++;
    }
    prevUnixtime = atol(prevUnixtime_chr);
    SD_filePeriodSms.close();
    Serial.print("curr: ");
    Serial.print(prevUnixtime, DEC);
    Serial.println(" ");
    res_isAllowByPeriod = (bool) (currUnixtime - prevUnixtime) > periodByTypeSms;
  }
  else {
    Serial.println("!open_read");
    res_SDisDamaged = true;
  }

  if (SD_filePeriodSms.open(filename, O_WRITE | O_CREAT | O_TRUNC )) {
    Serial.println("open_w");
    SD_filePeriodSms.println(currUnixtime);
    SD_filePeriodSms.close();
  }
  else {
    Serial.println("!open_w");
    res_SDisDamaged = true;
  }

  if (res_SDisDamaged) {
    return true; //sd damaged || create new => send SMS and send, balanse will be 0
  }
  else {
    return res_isAllowByPeriod;
  }
}


void setup() {
  Serial.begin(9600);
  delay(1000);
  if (SD_card.begin(10, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void loop() {
  String type = "wleak.sms";
  bool isAllow = conf_isAllowSMS(type);
  Serial.println(" isAllow" + isAllow);
  delay(2000);

}
