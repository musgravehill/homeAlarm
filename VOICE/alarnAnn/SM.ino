void STATEMACHINE_loop() {
  int32_t  STATEMACHINE_currMillis = millis();
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_250ms) > 250) {
    STATEMACHINE_250ms();
    STATEMACHINE_prevMillis_250ms = STATEMACHINE_currMillis;
  }
  if ((STATEMACHINE_currMillis - STATEMACHINE_prevMillis_5s) > 5000) {
    STATEMACHINE_5s();
    STATEMACHINE_prevMillis_5s = STATEMACHINE_currMillis;
  }
}

void STATEMACHINE_250ms() {
  uint8_t btn_mute_state = digitalRead(AN_btn_mute);
  if (btn_mute_state == 1) {
    AN_isAllowAlarm = false;
    AN_disallowAlarmMillis = millis();
    tmrpcm.disable();
  }
}

void STATEMACHINE_5s() {
  if (isAllowAlarm()) {
    AN_isAllowAlarm = false; //disallow music for next 62s
    AN_disallowAlarmMillis = millis(); //disallow music for next 62s
    tmrpcm.play("alarm.wav");
  }

  if ((millis() - AN_disallowAlarmMillis) > 62000) {
    AN_isAllowAlarm = true; //disallow music for 62s, then ALLOW
  }
}

