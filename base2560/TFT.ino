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
  String infoLine = "";
  gsmSerial.println("AT+CSQ");
  delay(80);
  //myDisplay.fillRect(0, 223, 320, 17, ILI9341_NAVY); //x y w h color
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setTextSize(2);
  //RTC
  DateTime now = RTC3231.now();
  uint32_t nowUt = now.unixtime();
  uint16_t yy =  now.year() - 2000;
  uint8_t mm = now.month();
  uint8_t dd =  now.day();
  uint8_t i =  now.minute();
  uint8_t H =  now.hour();
  float temperature = SYS_DS3231.getTemperature();

  myDisplay.setCursor(2, 2);
  myDisplay.setTextColor(ILI9341_WHITE);
  infoLine = ((dd < 10) ? "0" : "") + String(dd, DEC) + "." ;
  infoLine += ((mm < 10) ? "0" : "") + String(mm, DEC) + "." ;
  infoLine += ((yy < 10) ? "0" : "") + String(yy, DEC) + " " ;
  infoLine += ((H < 10) ? "0" : "") + String(H, DEC) + ":" ;
  infoLine += ((i < 10) ? "0" : "") + String(i, DEC);
  myDisplay.print(infoLine);
  myDisplay.setTextColor(ILI9341_PURPLE);
  infoLine = "  " + String(((int) temperature), DEC) + "C";
  myDisplay.print(infoLine);

  //uptime days
  myDisplay.setCursor(2, 12);
  myDisplay.setTextColor(ILI9341_BLUE);
  infoLine = "uptime " + String(((int) (millis() / 86400000L)), DEC) + " days";
  myDisplay.print(infoLine);

  //gsm
  if (GSM_answerCSQ.length() > 7) { //+CSQ: 23,0
    String s_tmp = GSM_answerCSQ.substring(6); //23,0
    uint8_t pos_tmp = s_tmp.indexOf(',');
    uint8_t gsm_RSSI_raw = s_tmp.substring(0, pos_tmp).toInt();
    uint8_t gsm_BER_raw = s_tmp.substring((pos_tmp + 1)).toInt();
    //int8_t gsmRSSI = (gsm_raw_RSSI == 99) ? 0 :  -115 + gsm_raw_RSSI * 2;

#ifdef DEBUG
    debugSerial.println("CSQ:" + GSM_answerCSQ);
    debugSerial.println("CSQvals:" + s_tmp);
    debugSerial.println("CSQ,:" + pos_tmp);
    debugSerial.println("CSQrssi:" + gsm_RSSI_raw);
    debugSerial.println("CSQber:" + gsm_BER_raw);
#endif

    if (gsm_RSSI_raw == 99) {
      myDisplay.setTextColor(ILI9341_RED);
      myDisplay.setCursor(2, 22);
      myDisplay.print("GSM NO SIGNAL");
    }
    else {
      myDisplay.fillRect(2, 22, 64, 10, ILI9341_LIGHTGREY); //x y w h color
      uint8_t gsmBarW = 64 * gsm_RSSI_raw / 31;
      if (gsm_RSSI_raw < 18) {
        myDisplay.fillRect(3, 23, gsm_RSSI_raw, 8, ILI9341_RED); //x y w h color
      }
      else if (gsm_RSSI_raw < 27) {
        myDisplay.fillRect(3, 23, gsm_RSSI_raw, 8, ILI9341_YELLOW); //x y w h color
      }
      else {
        myDisplay.fillRect(3, 23, gsm_RSSI_raw, 8, ILI9341_GREEN); //x y w h color
      }
    }

    if (gsm_BER_raw == 99) {
      myDisplay.setTextColor(ILI9341_RED);
      myDisplay.setCursor(160, 22);
      myDisplay.print("GSM ERROR UNKNOWN");
    }
    else {
      myDisplay.setTextColor(ILI9341_GREEN);
      myDisplay.setCursor(160, 22);
      myDisplay.print(gsm_BER_raw, DEC);
    }

  }




  myDisplay.setCursor(1, 225);
  myDisplay.setTextColor(ILI9341_WHITE);
  myDisplay.setTextSize(2);
  myDisplay.println(infoLine);
  //TODO
  //voltage base, acc
  //gsm signal bar

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
  //may be: set ATE1, ATV1, AT+CMEE=2
  gsmSerial.println("AT+COPS?");
  delay(80);
  gsmSerial.println("AT+CPAS");
  delay(80);
  gsmSerial.println("AT+CSQ");
  delay(80);

  myDisplay.fillScreen(ILI9341_WHITE);
  myDisplay.setTextColor(ILI9341_BLACK);
  myDisplay.setTextSize(1);

  myDisplay.setCursor(10, 2);
  myDisplay.println(GSM_answerCOPS);

  myDisplay.setCursor(10, 12);
  myDisplay.println(GSM_answerCPAS);

  myDisplay.setCursor(10, 22);  ;
  myDisplay.println(GSM_answerCSQ);

  myDisplay.setCursor(10, 32);
  myDisplay.println(GSM_answerCLIP);

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

