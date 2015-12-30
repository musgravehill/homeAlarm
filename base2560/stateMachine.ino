void STATEMACHINE_loop() {
  unsigned long STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_5s) > 5000) {
    STATEMACHINE_5s();
    STATEMACHINE_prevMillis_5s = STATEMACHINE_currMillis;
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

void STATEMACHINE_5s() {
  if (BASE_buzzerIsNeed) {
    SYS_DS3231.enableOscillator(true, false, 2);
    BASE_buzzerIsNeed = false;
  } else {
    SYS_DS3231.enableOscillator(false, false, 2);
  }
  
  TFT_checkBtnPower();
  TFT_renderInfoLine();
  TFT_renderSensors();
}

void STATEMACHINE_61s() {
  BASE_checkSensorsFault();
  //TODO function check DNGRs, periodSmsLimits and send SMS
}

void STATEMACHINE_103s() {
  GSM_cleanAllSMS();
}

