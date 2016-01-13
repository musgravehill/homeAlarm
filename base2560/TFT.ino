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
#ifdef DEBUG
  debugSerial.print("MENU_state=");
  debugSerial.println(MENU_state, DEC);
#endif

}

void TFT_renderInfoLine() {
  //myDisplay.fillRect(0, 223, 320, 17, ILI9341_NAVY); //x y w h color
  myDisplay.fillScreen(ILI9341_BLACK);
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
  myDisplay.fillScreen(ILI9341_BLACK);
  //table cells
  myDisplay.drawFastHLine(0, 40, 320, ILI9341_DARKGREY); //x,y,w,color
  myDisplay.drawFastHLine(0, 80, 320, ILI9341_DARKGREY); //x,y,w,color
  myDisplay.drawFastHLine(0, 120, 320, ILI9341_DARKGREY); //x,y,w,color
  myDisplay.drawFastHLine(0, 160, 320, ILI9341_DARKGREY); //x,y,w,color
  myDisplay.drawFastHLine(0, 200, 320, ILI9341_DARKGREY); //x,y,w,color

  myDisplay.drawFastVLine(40, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(80, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(120, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(160, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(200, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(240, 0, 240, ILI9341_DARKGREY); //x,y,h,color
  myDisplay.drawFastVLine(280, 0, 240, ILI9341_DARKGREY); //x,y,h,color

  myDisplay.setTextColor(ILI9341_WHITE);
  myDisplay.setTextSize(2);

  myDisplay.setCursor(4, 4);
  myDisplay.print("#");

  myDisplay.setCursor(44, 4);
  myDisplay.print("V");

  myDisplay.setCursor(84, 4);
  myDisplay.print("T");

  myDisplay.setCursor(124, 4);
  myDisplay.print("H");

  myDisplay.setCursor(164, 4);
  myDisplay.print("WL");

  myDisplay.setCursor(204, 4);
  myDisplay.print("CH4");

  myDisplay.setCursor(244, 4);
  myDisplay.print("MD");

  myDisplay.setCursor(284, 4);
  myDisplay.print("CO");

  //data
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) { //SENSORS PIPES 1..5!
    //sensor num
    if (BASE_sensorIsOn[sensorPipeNum]) {
      myDisplay.setTextColor(ILI9341_BLUE);
    } else {
      myDisplay.setTextColor(ILI9341_DARKGREY);
    }
    myDisplay.setCursor( 4, (4 + sensorPipeNum * 40));
    myDisplay.print(sensorPipeNum, DEC);

    //params
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsAvailable[sensorPipeNum][paramNum]) {
        if (BASE_sensorParamsIsDanger[sensorPipeNum][paramNum]) {
          myDisplay.setTextColor(ILI9341_RED);
        } else {
          myDisplay.setTextColor(ILI9341_OLIVE);
        }
        myDisplay.setCursor( (2 + 40 + 40 * paramNum), (2 + 40 * sensorPipeNum) );
        if (BASE_sensorDecodedParams[sensorPipeNum][paramNum] > 99) {
          myDisplay.setTextSize(1);
        }
        else {
          myDisplay.setTextSize(2);
        }
        myDisplay.print(BASE_sensorDecodedParams[sensorPipeNum][paramNum], DEC);
      }
    }
  }
}

void TFT_renderGSM() {
  myDisplay.fillScreen(ILI9341_WHITE);
  myDisplay.setTextColor(ILI9341_BLACK);
  myDisplay.setTextSize(1);

  //may be: set ATE1, ATV1, AT+CMEE=2

  myDisplay.setCursor(10, 2);
  gsmSerial.println("AT+COPS?");
  myDisplay.println(GSM_answerCOPS);

  myDisplay.setCursor(10, 12);
  gsmSerial.println("AT+CPAS");
  myDisplay.println(GSM_answerCPAS);

  myDisplay.setCursor(10, 22);
  gsmSerial.println("AT+CSQ");
  myDisplay.println(GSM_answerCSQ);


  /*
    AT+COPS?  +COPS: 0,0,"MTS-RUS"    Информация об операторе

    AT+CPAS  +CPAS:                   Информация о состояние модуля
                                    0 – готов к работе
                                    2 – неизвестно
                                    3 – входящий звонок
                                    4 – голосовое соединение

    AT+CSQ  +CSQ: 17,0          Уровень сигнала:
                              0 -115 дБл и меньше
                              1 -112 дБл
                              2-30 -110..-54 дБл
                              31 -52 дБл и сильнее
                              99 – нет сигнала.
                              ---
                              0 - коэффициент ошибок связи (т.н. RXQUAL).
                              Может быть от 0 до 7, чем число меньше тем качество связи лучше.
  */
}

