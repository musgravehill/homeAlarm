// *  TFT SPI ILI9341
//    LCD nokia 5110 PCD8544


void TFT_init() {
  myDisplay.begin();
  myDisplay.setRotation(0);
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.setTextColor(ILI9341_GREEN);
  myDisplay.setTextSize(1);
  myDisplay.println("Boris homeAlarm v1.0");
}

void TFT_renderSensors() {
  //bool BASE_sensorIsOk[6] = {false}; //0 1..5
  //uint16_t BASE_sensorParams[6][7] = {0}; //encoded uint params; 0==null
  //DECODE params before display it
}



