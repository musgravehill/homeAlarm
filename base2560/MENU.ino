void ENCODER_read() {
  MENU_state = myEncoder.read();
  if (MENU_state < 0) {
    MENU_state = 0;
    myEncoder.write(0);
  }
  if (MENU_state > 12) {
    MENU_state = 12;
    myEncoder.write(12);
  } 
  MENU_state = 4; //DBG
}
