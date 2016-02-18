void ALARM_processSensorsParams() {
  BASE_buzzer_isNeed = false;
  BASE_siren_isNeed = false;

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
          if (paramNum == 5) {
            BASE_siren_isNeed = true;
          }
          //for testing
          BASE_buzzer_isNeed = true;
        }
        else {
          BASE_buzzer_isNeed = true;
        }
      }
    }
  }
}

void ALARM_beepShort_buzzer() {
  digitalWrite(BASE_buzzer_pin, 1);
  delay(100);
  digitalWrite(BASE_buzzer_pin, 0);
}
void ALARM_beepShort_siren() {
  digitalWrite(BASE_siren_pin, 1);
  delay(1000);
  digitalWrite(BASE_siren_pin, 0);
}
void ALARM_controlBuzzer() {
  if (BASE_buzzer_isNeed) {
    ALARM_beepShort_buzzer();
  }
}
void ALARM_controlSiren() {
  if (BASE_siren_isNeed) {
    digitalWrite(BASE_siren_pin, 1);
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



