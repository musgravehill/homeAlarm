// *  TFT SPI ILI9341
//    LCD nokia 5110 PCD8544


void TFT_init() {
  pinMode(TFT_pinLedPower, OUTPUT);
  myDisplay.begin();
  myDisplay.setRotation(1);
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setCursor(50, 4);
  myDisplay.setTextColor(ILI9341_GREEN);
  myDisplay.setTextSize(2);
  myDisplay.println("Boris homeAlarm v1.0");
}

void TFT_initLED() {
  if (MENU_state > 0) {
    digitalWrite(TFT_pinLedPower, 1);
  }
  else {
    digitalWrite(TFT_pinLedPower, 0);
  }
}

void TFT_renderMenuState() {
  switch (MENU_state) {
    case 0:
      //no TFT render, TFT_LED will be off
      break;
    case 4:
      TFT_renderInfoLine();
      break;
    case 8:
      TFT_renderSensors();
      break;
    case 12:
      TFT_renderGSM();
      break;
  }

}

void TFT_renderInfoLine() {
  myDisplay.fillRect(0, 223, 320, 17, ILI9341_NAVY); //x y w h color
  DateTime now = RTC3231.now();
  uint32_t nowUt = now.unixtime();
  uint16_t yy =  now.year() - 2000;
  uint8_t mm = now.month();
  uint8_t dd =  now.day();
  uint8_t i =  now.minute();
  uint8_t H =  now.hour();
  //float temperature = SYS_DS3231.getTemperature();
  String infoLine = String(dd, DEC) + "." + String(mm, DEC) + "." + String(yy, DEC) + " " + String(H, DEC) + ":" + String(i, DEC);
  //infoLine += " " + String(((int) temperature), DEC) + "C";
  infoLine += " " + String(((int) (millis() / 86400000L)), DEC) + "d"; //uptime days
  myDisplay.setCursor(1, 225);
  myDisplay.setTextColor(ILI9341_WHITE);
  myDisplay.setTextSize(2);
  myDisplay.println(infoLine);
  //TODO
  //diz
  //voltage base acc
  //gsm signal bar
  //temperature base
}

void TFT_renderSensors() {
  //BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum]
  //BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum]
  //BASE_sensorDecodedParams[NRF_currPipeNum][paramNum]
  //BASE_sensorIsOk[NRF_currPipeNum]

  myDisplay.fillRect(0, 0, 320, 240, ILI9341_BLACK); //x y w h color
  
  myDisplay.setCursor(1, 30);
  myDisplay.setTextColor(ILI9341_PINK);
  myDisplay.setTextSize(2);
  myDisplay.println("sensors");
}

void TFT_renderGSM() {
  myDisplay.fillRect(0, 0, 320, 222, ILI9341_WHITE); //x y w h color
  myDisplay.setCursor(1, 30);
  myDisplay.setTextColor(ILI9341_PINK);
  myDisplay.setTextSize(2);
  myDisplay.println("gsm________");
}

