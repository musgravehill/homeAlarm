/*
  SD structure
  log.txt     all logs here
  phones.txt  phones for sms, call, etc
  ddmmyy.***  GSM conf log
  NAME            EXT     PERIOD_SECONDS
  CH4             .ch4    
  humidity        .h      
  motionDetector  .mov    
  powerUp         .pup    
  sensorVoltage   .v      24*3600 [1 sms per 1 day]      
  temperature     .t      
  waterLeak       .wl     
*/

#include <SPI.h>
#include <SdFat.h>
SdFat SD_card;
SdFile SD_confFileGSM;
bool SD_isEnable = false;

String RTC_getDDMMYY() {
  //TODO, if size()==1 => "0"+ *
  return "010502"; //01-05-2002
}

bool conf_isAllowSMS() {
  //
  String currDDMMYY = RTC_getDDMMYY();
  String filename_str = String(currDDMMYY + ".gsm");
  char filename_chr[sizeof(filename_str)];
  filename_str.toCharArray(filename_chr, sizeof(filename_chr));

  if (!SD_isEnable) {
    return false;
  }
  if (!SD_confFileGSM.open(filename_chr, O_READ)) {
    if (!SD_confFileGSM.open(filename_chr, O_WRITE | O_CREAT )) {
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
