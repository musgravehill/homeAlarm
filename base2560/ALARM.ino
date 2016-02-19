void ALARM_processSensorsParams() {
  ALARM_SOFT_isNeed = false;
  ALARM_LOUD_isNeed = false;

  for (uint8_t sensorNum = 1; sensorNum < 6; sensorNum++) { //SENSORS PIPES 1..5!
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsDanger[sensorNum][paramNum]) {
        if (BASE_ALARM_MOTION_MODE) { //ALARM MODE: SMS, SIREN
          //SMS
          if (GSM_paramIsAllowSms(paramNum)) {
            String SMS_danger =  "#" + String(sensorNum, DEC) + " ";
            SMS_danger +=  PARAMS_getVerbalParamName(paramNum) + "=";
            SMS_danger +=  String(BASE_sensorDecodedParams[sensorNum][paramNum], DEC) + " ";
            GSM_addToQueueSMS_forAllPhones(SMS_danger);
          }
          //siren
          if (paramNum == 5) { //MOTION DETECTOR
            ALARM_LOUD_isNeed = true;
          }
        }
        else { //owner at home, NO SMS||SIREN
          ALARM_SOFT_isNeed = true;
        }
      }
    }
  }
}

void ALARM_controlIndication() {
  uint32_t currMillis = millis();
  uint32_t durationOn = currMillis - ALARM_indication_startTime;
  if (durationOn > 23000)  { //duration, when siren||beeper is ON
    INTERFACE_siren_off(); //siren **seconds, then off  => saves battery power
  }
  if (ALARM_LOUD_isNeed) {
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



