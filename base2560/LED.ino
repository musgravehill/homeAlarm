/*
void LED_init() {
  pinMode(LED_latchPin, OUTPUT);
  pinMode(LED_dataPin, OUTPUT);
  pinMode(LED_clockPin, OUTPUT);
  
  LED_twoBytes = 0b1111111111111111; //TEST LEDS
  digitalWrite(LED_latchPin, LOW); //leds off
  byte LED_twoByte1 = highByte(LED_twoBytes);
  byte LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
}

void LED_paramsState() {
  //0=null  1=ok 2=danger
  LED_twoBytes = 0b0; //ALL LEDS OFF
  for (uint8_t i = 0; i < 7; i++) {
    switch (BASE_isDangerParams[i]) {
      case 0:   //0=null
        break;
      case 1:   //1=ok
        bitWrite(LED_twoBytes, (i * 2), 1); //green on
        break;
      case 2:   //2=danger
        bitWrite(LED_twoBytes, (i * 2 + 1), 1); //red on
        break;
    }
  }

  digitalWrite(LED_latchPin, LOW); //leds off
  byte LED_twoByte1 = highByte(LED_twoBytes);
  byte LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
}
*/
