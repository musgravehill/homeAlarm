void STATEMACHINE_loop() {
  int32_t  STATEMACHINE_currMillis = millis();
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
  wdt_reset();
  TFT_initLED();
  wdt_reset();
  GSM_listenSerial();
  wdt_reset();
  GSM_checkIncomingCall();
  wdt_reset();
}

void STATEMACHINE_17s() {
  wdt_reset();
  GSM_ping();
  wdt_reset();
  TFT_renderMenuState();
  wdt_reset();
}

void STATEMACHINE_61s() {
  wdt_reset();
  BASE_checkSensorsFault();
  wdt_reset();
  GSM_queueLoop_stateMachine_processing();
  wdt_reset();
}

void STATEMACHINE_103s() {
  wdt_reset();
  GSM_cleanAllSMS();
  wdt_reset();

  //reset base after 1 day uptime
  if ((uint32_t) millis() > 86400000L) {
    wdt_disable();
    delay(20);
    wdt_enable(WDTO_2S);
    delay(2100);
  }
}

