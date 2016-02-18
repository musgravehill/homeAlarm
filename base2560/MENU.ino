void MENU_btnsCheck() {
  if (digitalRead(MENU_btnPrev_pin) == 1) {
    ALARM_beepShort_buzzer();
    MENU_state--;
    if (MENU_state < 0) {
      MENU_state = 0;
    }
    TFT_renderMenuState();
  }

  if (digitalRead(MENU_btnNext_pin) == 1) {
    ALARM_beepShort_buzzer();
    MENU_state++;
    if (MENU_state > 3) {
      MENU_state = 3;
    }
    TFT_renderMenuState();
  }
}





