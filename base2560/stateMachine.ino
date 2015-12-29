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
}
