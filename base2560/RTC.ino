void RTC_setTime() {
  DS3231 SYS_DS3231;
  
  SYS_DS3231.setYear(15);
  SYS_DS3231.setMonth(12);
  SYS_DS3231.setDate(30);
  SYS_DS3231.setDoW(2); //Sets the Day of the Week (1-7);
  SYS_DS3231.setHour(19);
  SYS_DS3231.setMinute(55);
  SYS_DS3231.setSecond(0);
}
