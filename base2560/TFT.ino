//TFT SPI ILI9341
//LCD nokia 5110 PCD8544


void TFT_init() {
  myDisplay.begin();
  myDisplay.setRotation(0);
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.setTextColor(ILI9341_GREEN);
  myDisplay.setTextSize(1);
  myDisplay.println("Boris homeAlarm v1.0");
}

