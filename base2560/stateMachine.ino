void STATEMACHINE_loop() {
  int32_t  STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_300ms) > 300L) {
    STATEMACHINE_300ms();
    STATEMACHINE_prevMillis_300ms = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_1s) > 1000L) {
    STATEMACHINE_1s();
    STATEMACHINE_prevMillis_1s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_3s) > 3000L) {
    STATEMACHINE_3s();
    STATEMACHINE_prevMillis_3s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_5s) > 5000L) {
    STATEMACHINE_5s();
    STATEMACHINE_prevMillis_5s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_17s) > 17000L) {
    STATEMACHINE_17s();
    STATEMACHINE_prevMillis_17s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_61s) > 61000L) {
    STATEMACHINE_61s();
    STATEMACHINE_prevMillis_61s = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_103s) > 103000L) {
    STATEMACHINE_103s();
    STATEMACHINE_prevMillis_103s = STATEMACHINE_currMillis;
  }
}

void STATEMACHINE_300ms() {
  wr();
  MENU_btnsCheck();
  wr();
}

void STATEMACHINE_1s() {
  wr();
  TFT_initLED();
  wr();
  GSM_listenSerial();
  wr();
  GSM_checkIncomingCall();
  wr();
  ALARM_controlSiren();
  wr();
}

void STATEMACHINE_3s() {
  wr();
  ALARM_processSensorsParams();
  wr();
}

void STATEMACHINE_5s() {
  wr();
  GSM_queueLoopSMS_stateMachine_processing();//SMS are added to queue. Processing 1 sms at one time
  wr();
  ALARM_controlBuzzer();
  wr();

}

void STATEMACHINE_17s() {
  wr();
  TFT_renderMenuState();
  wr();
  GSM_ping();
  wr();
  GSM_pingCheckTimeAnswer();
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
  //what about ALARM_MODE? I NEED SOME "BTN" WITH FIXATION => SET MODE AFTER REBOOT AGAIN
  //MAY BE EEPROM???
  /*if ((uint32_t) millis() > 86400000L) {
    wdt_disable();
    delay(20);
    wdt_enable(WDTO_2S);
    delay(2100);
    }*/
}

