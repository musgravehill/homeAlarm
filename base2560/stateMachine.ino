void STATEMACHINE_loop() {
  int32_t  STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_1s) > 1000) {
    STATEMACHINE_1s();
    STATEMACHINE_prevMillis_1s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_3s) > 3000) {
    STATEMACHINE_3s();
    STATEMACHINE_prevMillis_3s = STATEMACHINE_currMillis;
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
  wr();
  TFT_initLED();
  wr();
  GSM_listenSerial();
  wr();
  GSM_checkIncomingCall();
  wr();
}

void STATEMACHINE_3s() {
  wr();
  GSM_initSmsDangers();
  wr();
}

void STATEMACHINE_17s() {
  wr();
  GSM_queueLoopSMS_stateMachine_processing();//SMS are added to queue. Processing 1 sms at one time
  wr();
  GSM_pingCheckTimeAnswer();
  wr();
  GSM_ping();
  wr();
  TFT_renderMenuState();
  wr();
}

void STATEMACHINE_61s() {
  wr();
  BASE_checkSensorsFault();
  wr();

}

void STATEMACHINE_103s() {
  wr();
  GSM_cleanAllSMS();
  wr();

  //reset base after 1 day uptime
  if ((uint32_t) millis() > 86400000L) {
    wdt_disable();
    delay(20);
    wdt_enable(WDTO_2S);
    delay(2100);
  }
}

