void ALARM_processSensorsParams() {
  ALARM_buzzer_isNeed = false;
  ALARM_siren_isNeed = false;

  for (uint8_t sensorNum = 1; sensorNum < 6; sensorNum++) { //SENSORS PIPES 1..5!
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsDanger[sensorNum][paramNum]) {
        if (BASE_ALARM_MOTION_MODE) { //ALRM MODE && param_M_5
          //SMS
          if (GSM_paramIsAllowSms(paramNum)) {
            String SMS_danger =  "#" + String(sensorNum, DEC) + " ";
            SMS_danger +=  PARAMS_getVerbalParamName(paramNum) + "=";
            SMS_danger +=  String(BASE_sensorDecodedParams[sensorNum][paramNum], DEC) + " ";
            GSM_addToQueueSMS_forAllPhones(SMS_danger);
          }
          //siren
          if (paramNum == 5) { //MOTION DETECTOR
            ALARM_siren_isNeed = true;
          }
          ALARM_buzzer_isNeed = true;//for testing
        }
        else {
          ALARM_buzzer_isNeed = true;
        }
      }
    }
  }
}

void ALARM_controlIndication() {
  uint32_t currMillis = millis();
  uint32_t delta = millis() - ALARM_indication_startTime;
  if (
    (ALARM_siren_isNeed || ALARM_buzzer_isNeed) &&
    (ALARM_indication_startTime == 0) || (delta < 3000)  || (delta > 5000)
  )
  {
    ALARM_indication_startTime = currMillis;

    if (ALARM_siren_isNeed) {
      INTERFACE_siren_on();
      INTERFACE_led_alarm_blink();
    }
    if (ALARM_buzzer_isNeed) {
      INTERFACE_buzzer_beep();
      INTERFACE_led_alarm_blink();
    }

  } else {
    ALARM_indication_endTime = currMillis;
  }
}

//TODO
void ALARM_save_alarmModeState() {
  if (BASE_ALARM_MOTION_MODE) {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B1);
  } else {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B0);
  }
}

void ALARM_restore_alarmModeState() {
  uint8_t alarmStateFromEeprom = eeprom24C32.readByte(eeprom24C32_address_alarmMode);
  BASE_ALARM_MOTION_MODE = (alarmStateFromEeprom == 1) ? true : false;
}



