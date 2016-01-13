void STATEMACHINE_loop() {
  uint32_t  STATEMACHINE_currMillis = millis(); 
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_1s) > 1000) {
    STATEMACHINE_1s();
    STATEMACHINE_prevMillis_1s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_17s) > 17000) {
    STATEMACHINE_17s();
    STATEMACHINE_prevMillis_17s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_61s) > 61000) {
    STATEMACHINE_61s();
    STATEMACHINE_prevMillis_61s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_103s) > 103000) {
    STATEMACHINE_103s();
    STATEMACHINE_prevMillis_103s = STATEMACHINE_currMillis;
  }
}

void STATEMACHINE_1s() {
  TFT_initLED();
  GSM_listenSerial();
  GSM_checkIncomingCall();

#ifdef DEBUG
  debugSerial.print("GSM_phoneNums_count=");
  debugSerial.println(GSM_phoneNums_count, DEC);
  for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
    debugSerial.print(i, DEC);
    debugSerial.print("___");
    debugSerial.print(GSM_phoneNums[i]);
    debugSerial.println("___");
  }
#endif
}

void STATEMACHINE_17s() {
  TFT_renderMenuState();

  if (BASE_buzzerIsNeed) {
    BASE_buzzerIsNeed = false;
  }

}

void STATEMACHINE_61s() {
  BASE_checkSensorsFault();
}

void STATEMACHINE_103s() {
  GSM_cleanAllSMS();
}

