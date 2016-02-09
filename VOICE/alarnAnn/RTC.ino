void RTC_init() {
  SYS_DS3231.enableOscillator(true, true, 0);
  SYS_DS3231.setClockMode(true); //ERROR? true=24 false=12? // Set 12/24h mode. True is 12-h, false is 24-hour.
  delay(20);
}

void RTC_setTimeFromSD() {
  //IN FILE:   yy.mm.dd.hh.ii.ss.dow.
  if (SD_isEnable) {
    if (SD_file.open("dt.txt", O_READ)) { //8.3 filename.ext rule
      char chr;
      String val = "";
      uint8_t i = 0;
      while (SD_file.available()) {
        chr = SD_file.read();
        if ( chr == '0'
             || chr == '1'
             || chr == '2'
             || chr == '3'
             || chr == '4'
             || chr == '5'
             || chr == '6'
             || chr == '7'
             || chr == '8'
             || chr == '9'
           )
        {
          val += String((char)chr);
        }
        if (chr == '.') {
          RTC_processDataSD(val, i);
          val = "";
          i++;
        }
      }
      //SD_file.close(); //dont close before delete
      SD_file.remove();
    }

  }
}

void RTC_processDataSD(String val, uint8_t i) {
  //IN FILE:   yy.mm.dd.hh.ii.ss.dow.
  uint8_t dtChunk = val.toInt();
  switch (i) {
    case 0:
      SYS_DS3231.setYear(dtChunk);
      break;
    case 1:
      SYS_DS3231.setMonth(dtChunk);
      break;
    case 2:
      SYS_DS3231.setDate(dtChunk);
      break;
    case 3:
      SYS_DS3231.setHour(dtChunk);
      break;
    case 4:
      SYS_DS3231.setMinute(dtChunk);
      break;
    case 5:
      SYS_DS3231.setSecond(dtChunk);
      break;
    case 6:
      SYS_DS3231.setDoW(dtChunk);
      break;
  }
}
