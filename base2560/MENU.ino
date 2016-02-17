void MENU_btnsCheck() {
  if (digitalRead(MENU_btnPrev_pin) == 1) {
    MENU_state--;
    if (MENU_state < 0) {
      MENU_state = 0;
    }
  }

  if (digitalRead(MENU_btnNext_pin) == 1) {
    MENU_state++;
    if (MENU_state > 3) {
      MENU_state = 3;
    }
  }
}





