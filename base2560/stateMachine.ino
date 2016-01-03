void STATEMACHINE_loop() {
  unsigned long STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_2s) > 2000) {
    STATEMACHINE_2s();
    STATEMACHINE_prevMillis_2s = STATEMACHINE_currMillis;
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

void STATEMACHINE_2s() {
  if (BASE_buzzerIsNeed) {
    BASE_buzzerIsNeed = false;
  } else {

  }

  TFT_renderState();

#ifdef DEBUG
  debugSerial.println(MENU_state, DEC);
#endif
}

void STATEMACHINE_61s() {
  BASE_checkSensorsFault();
  //TODO function check DNGRs, periodSmsLimits and send SMS
}

void STATEMACHINE_103s() {
  GSM_cleanAllSMS();
}

