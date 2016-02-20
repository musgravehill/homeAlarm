void MENU_btnsCheck() {
  if (digitalRead(INTERFACE_btn_prev_pin) == 1) {
    INTERFACE_buzzer_beep();
    INTERFACE_led_alarm_blink();
    MENU_state--;
    if (MENU_state < 0) {
      MENU_state = 0;
    }
    TFT_renderMenuState();
  }

  if (digitalRead(INTERFACE_btn_next_pin) == 1) {
    INTERFACE_buzzer_beep();
    INTERFACE_led_alarm_blink();
    MENU_state++;
    if (MENU_state > 5) {
      MENU_state = 5;
    }
    TFT_renderMenuState();
  }

  if (digitalRead(INTERFACE_btn_reset_pin) == 1) {
    INTERFACE_buzzer_beep();
    INTERFACE_led_alarm_blink();
    wdt_disable();
    delay(20);
    wdt_enable(WDTO_2S);
    delay(2100);
  }

}





