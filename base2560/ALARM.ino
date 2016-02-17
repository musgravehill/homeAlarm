void ALARM_save_alarmModeState() {
  if (BASE_ALARM_MODE) {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B1);
  } else {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B0);
  }
}

void ALARM_restore_alarmModeState() {
  uint8_t alarmStateFromEeprom = eeprom24C32.readByte(eeprom24C32_address_alarmMode);
  BASE_ALARM_MODE = (alarmStateFromEeprom == 1) ? true : false;
}

void ALARM_beepShort_buzzer() {
  digitalWrite(BASE_buzzer_pin, 1);
  delay(1000);
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

