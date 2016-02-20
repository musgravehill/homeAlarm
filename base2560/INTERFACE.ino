void INTERFACE_buzzer_beep() {
  digitalWrite(INTERFACE_buzzer_pin, 1);
  delay(77);
  digitalWrite(INTERFACE_buzzer_pin, 0);
}
void INTERFACE_siren_beep() {
  digitalWrite(INTERFACE_siren_pin, 1);
  delay(500);
  digitalWrite(INTERFACE_siren_pin, 0);
}
void INTERFACE_siren_on() {
  digitalWrite(INTERFACE_siren_pin, 1);
}
void INTERFACE_siren_off() {
  digitalWrite(INTERFACE_siren_pin, 0);
}

void INTERFACE_led_alarm_blink() {
  digitalWrite(INTERFACE_led_alarm_pin, 1);
  delay(100);
  digitalWrite(INTERFACE_led_alarm_pin, 0);
}
