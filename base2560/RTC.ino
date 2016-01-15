void RTC_setTime() {  
  SYS_DS3231.setYear(16);
  SYS_DS3231.setMonth(1);
  SYS_DS3231.setDate(15);
  SYS_DS3231.setDoW(2); //Sets the Day of the Week (1-7);
  SYS_DS3231.setHour(22);
  SYS_DS3231.setMinute(14);
  SYS_DS3231.setSecond(0);
  SYS_DS3231.enableOscillator(true, true, 0);
}

//TODO  SET DATETIME FROM SD: if isset dt.txt => read it & set RTC & delete file.
