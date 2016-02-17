void ALARM_save_alarmModeState() {
  if (BASE_ALARM_MODE) {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B1);
  } else {
    eeprom24C32.writeByte(eeprom24C32_address_alarmMode, B0);
  }
  //eeprom.readByte(address);
}


