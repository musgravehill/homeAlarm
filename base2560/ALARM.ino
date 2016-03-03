void ALARM_processSensorsParams() {
  ALARM_SOFT_isNeed = false;
  ALARM_LOUD_isNeed = false;

  for (uint8_t sensorNum = 1; sensorNum < 6; sensorNum++) { //SENSORS PIPES 1..5!
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsDanger[sensorNum][paramNum]) {
        //SMS
        if (GSM_paramIsAllowSms(paramNum)) {
          String SMS_danger =  "#" + String(sensorNum, DEC) + " ";
          SMS_danger +=  PARAMS_getVerbalParamName(paramNum) + "=";
          SMS_danger +=  String(BASE_sensorDecodedParams[sensorNum][paramNum], DEC) + " ";
          GSM_addToQueueSMS_forAllPhones(SMS_danger);
        }
        if (ALARM_SECURITY_MODE) {
          if (paramNum == 5) { //MOTION DETECTOR
            ALARM_LOUD_isNeed = true; //siren
            ALARM_SECURITY_initTime = (ALARM_SECURITY_initTime == 0) ? millis() : ALARM_SECURITY_initTime;
          }
        }
        else {
          ALARM_SOFT_isNeed = true; //beep
        }
      }
    }
  }
}

void ALARM_controlIndication() {
  uint32_t currMillis = millis();
  uint32_t durationOn = currMillis - ALARM_indication_startTime;
  if (durationOn > 23000)  { //duration, when siren||beeper is ON
    INTERFACE_siren_off(); //siren is on ** seconds, then off  => saves battery power
  }
  if ( ALARM_LOUD_isNeed && ((currMillis - ALARM_SECURITY_initTime) > 5000)  ) {
    INTERFACE_siren_on();
    INTERFACE_led_alarm_blink();
    ALARM_indication_startTime = currMillis;
  }
  if (ALARM_SOFT_isNeed) {
    INTERFACE_buzzer_beep();
    INTERFACE_led_alarm_blink();
    ALARM_indication_startTime = currMillis;
  }
}


void ALARM_set_alarmModeState() {
  if (ALARM_SECURITY_MODE) {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B1);
  } else {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B0);
  }
}

void ALARM_restore_alarmModeState() {
  uint8_t alarmStateFromEeprom = eeprom24C32.readByte(eeprom24C32_address_alarmMode);
  ALARM_SECURITY_MODE = (alarmStateFromEeprom == B1) ? true : false;
}



