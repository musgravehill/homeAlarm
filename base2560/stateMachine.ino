void STATEMACHINE_loop() {
  unsigned long STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_1s) > 1000) {
    STATEMACHINE_1s();
    STATEMACHINE_prevMillis_1s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_7s) > 7000) {
    STATEMACHINE_7s();
    STATEMACHINE_prevMillis_7s = STATEMACHINE_currMillis;
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
#ifdef DEBUG
  //debugSerial.print("MENU_state=");
  //debugSerial.println(MENU_state, DEC);
#endif
}

void STATEMACHINE_7s() {
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

