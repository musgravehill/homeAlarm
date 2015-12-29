void STATEMACHINE_loop() {
  unsigned long STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_5s) > 5000) {
    STATEMACHINE_5s();
    STATEMACHINE_prevMillis_5s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_10s) > 10000) {
    STATEMACHINE_10s();
    STATEMACHINE_prevMillis_10s = STATEMACHINE_currMillis;
  }
}

void STATEMACHINE_5s() {
  BASE_checkSensorsFault();
  TFT_renderSensors();


#ifdef DEBUG
  DateTime now = RTC3231.now();
  uint32_t nowUt = now.unixtime();
  debugSerial.println(nowUt, DEC);
  uint16_t yyyy =  now.year();
  uint8_t mm = now.month();
  uint8_t dd =  now.day();
  String filename_s = String(dd, DEC) + "." + String(mm, DEC) + "." + String(yyyy, DEC) + ".csv";
  debugSerial.println(filename_s);
#endif
}

void STATEMACHINE_10s() {
  //may be GSM_SMS
}
