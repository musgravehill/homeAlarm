// *  TFT SPI ILI9341
//    LCD nokia 5110 PCD8544


void TFT_init() {
  pinMode(TFT_btnPowerPin, INPUT);
  myDisplay.begin();
  myDisplay.setRotation(3);
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setCursor(50, 4);
  myDisplay.setTextColor(ILI9341_GREEN);
  myDisplay.setTextSize(2);
  myDisplay.println("Boris homeAlarm v1.0");
}

void TFT_renderInfoLine() {
  if (TFT_isOn) {
    myDisplay.fillRect(0, 223, 320, 17, ILI9341_NAVY); //x y w h color
    DateTime now = RTC3231.now();
    uint32_t nowUt = now.unixtime();
    uint16_t yy =  now.year() - 2000;
    uint8_t mm = now.month();
    uint8_t dd =  now.day();
    uint8_t i =  now.minute();
    uint8_t H =  now.hour();
    float temperature = SYS_DS3231.getTemperature();
    String infoLine = String(dd, DEC) + "." + String(mm, DEC) + "." + String(yy, DEC) + " " + String(H, DEC) + ":" + String(i, DEC);
    infoLine += " " + String(((int) temperature), DEC) + "C";
    myDisplay.setCursor(1, 225);
    myDisplay.setTextColor(ILI9341_WHITE);
    myDisplay.setTextSize(2);
    myDisplay.println(infoLine);
  }
}

void TFT_renderSensors() {
  if (TFT_isOn) {
  }
  //bool BASE_sensorIsOk[6] = {false}; //0 1..5
  //uint16_t BASE_sensorParams[6][7] = {0}; //encoded uint params; 0==null; [sensorNum][paramNum] 1..5  0..6
  //bool BASE_sensorParamsIsDanger[sensorNum][paramNum] = false; [6][7]  1..5  0..6
  //DECODE params before display it
}

void TFT_checkBtnPower() {
  uint8_t TFT_btnState = digitalRead(TFT_btnPowerPin);
  TFT_isOn = (bool) (TFT_btnState == 1);
}




