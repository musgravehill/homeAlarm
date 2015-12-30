void RTC_setTime() {  
  SYS_DS3231.setYear(15);
  SYS_DS3231.setMonth(12);
  SYS_DS3231.setDate(30);
  SYS_DS3231.setDoW(2); //Sets the Day of the Week (1-7);
  SYS_DS3231.setHour(23);
  SYS_DS3231.setMinute(11);
  SYS_DS3231.setSecond(0);
  SYS_DS3231.enableOscillator(true, true, 0);
}
