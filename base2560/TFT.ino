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
    case 1:
      TFT_renderInfoLine();
      break;
    case 2:
      TFT_renderSensors();
      break;
    case 3:
      TFT_renderGSM();
      break;
    case 4:
      TFT_renderPWR();
      break;
    case 5:
      TFT_renderTestAlarm();
      break;
  }
#ifdef DEBUG
  debugSerial.print("MENU_state=");
  debugSerial.println(MENU_state, DEC);
#endif

}

void TFT_renderTestAlarm() {
  myDisplay.fillScreen(ILI9341_PINK);
  myDisplay.setTextSize(3);
  myDisplay.setTextColor(ILI9341_BLACK);
  myDisplay.setCursor(2, 2);
  myDisplay.println("TEST: BUZZER, SIREN, LED");

  INTERFACE_buzzer_beep();
  INTERFACE_siren_beep();
  INTERFACE_led_alarm_blink();
}

void TFT_renderPWR() {
  String infoLine = "";
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setTextSize(3);
  myDisplay.setTextColor(ILI9341_WHITE);
  myDisplay.setCursor(2, 2);

  infoLine = "BASE: ";
  infoLine += String((BASE_voltage_base / 100), DEC) + " V" ;
  myDisplay.println(infoLine);

  infoLine = "ACDC: ";
  infoLine += String((BASE_voltage_acdc / 100), DEC) + " V" ;
  myDisplay.println(infoLine);

  infoLine = "BATT: ";
  infoLine += String((BASE_voltage_battery / 100), DEC) + " V" ;
  myDisplay.println(infoLine);

}

void TFT_renderInfoLine() {
  String infoLine = "";
  //myDisplay.fillRect(0, 223, 320, 17, ILI9341_NAVY); //x y w h color
  myDisplay.fillScreen(ILI9341_BLACK);
  myDisplay.setTextSize(2);
  //RTC
  DateTime now = RTC3231.now();
  //uint32_t nowUt = now.unixtime();
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
  myDisplay.setTextColor(ILI9341_GREEN);
  infoLine = "  " + String(((int)temperature), DEC) + "C";
  myDisplay.print(infoLine);

  //uptime days
  myDisplay.setCursor(2, 22);
  myDisplay.setTextColor(ILI9341_BLUE);
  infoLine = "uptime " + String((millis() / 86400000L), DEC) + " d";
  myDisplay.print(infoLine);

  //gsm
  if (GSM_answerCSQ.length() > 7) { //+CSQ: 23,0
    String s_tmp = GSM_answerCSQ.substring(6); //23,0
    uint8_t pos_tmp = s_tmp.indexOf(',');
    uint8_t gsm_RSSI_raw = s_tmp.substring(0, pos_tmp).toInt();
    uint8_t gsm_BER_raw = s_tmp.substring((pos_tmp + 1)).toInt();
    int8_t gsmRSSI = (gsm_RSSI_raw == 99) ? 0 :  -115 + gsm_RSSI_raw * 2;

    //#ifdef DEBUG
    //debugSerial.println("CSQ:" + GSM_answerCSQ);
    //debugSerial.println("CSQvals:" + s_tmp);
    //debugSerial.println("CSQ,:" + String(pos_tmp, DEC));
    //debugSerial.println("CSQrssi:" +  String(gsm_RSSI_raw, DEC));
    //debugSerial.println("CSQber:" + String(gsm_BER_raw, DEC));
    //#endif

    if (gsm_RSSI_raw == 99) {
      myDisplay.setTextColor(ILI9341_RED);
      myDisplay.setCursor(2, 42);
      myDisplay.print("GSM NO SIGNAL");
    }
    else {
      myDisplay.fillRect(2, 42, 64, 10, ILI9341_LIGHTGREY); //x y w h color
      uint8_t gsmBarW = 64 * gsm_RSSI_raw / 31;
      if (gsm_RSSI_raw < 18) {
        myDisplay.fillRect(3, 43, gsm_RSSI_raw, 8, ILI9341_RED); //x y w h color
        myDisplay.setTextColor(ILI9341_RED);
      }
      else if (gsm_RSSI_raw < 27) {
        myDisplay.fillRect(3, 43, gsm_RSSI_raw, 8, ILI9341_YELLOW); //x y w h color
        myDisplay.setTextColor(ILI9341_YELLOW);
      }
      else {
        myDisplay.fillRect(3, 43, gsm_RSSI_raw, 8, ILI9341_GREEN); //x y w h color
        myDisplay.setTextColor(ILI9341_GREEN);
      }
      myDisplay.setCursor(68, 42);
      myDisplay.print(gsmRSSI);
      myDisplay.print(" dBm");
    }

    if (gsm_BER_raw == 99) {
      myDisplay.setTextColor(ILI9341_RED);
      myDisplay.setCursor(160, 42);
      myDisplay.print("GSM ERROR UNKNOWN");
    }
    else {
      myDisplay.setTextColor(ILI9341_GREEN);
      myDisplay.setCursor(160, 42);
      myDisplay.print("error ");
      myDisplay.print(gsm_BER_raw, DEC);
    }
  }

  //phones
  myDisplay.setTextColor(ILI9341_MAGENTA);
  myDisplay.setCursor(2, 62);
  for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
    myDisplay.print(GSM_phoneNums[i] + " ");
  }

  //TODO
  //voltage base, acc

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

  myDisplay.setCursor(4, 16);
  myDisplay.print("#");

  myDisplay.setCursor(44, 16);
  myDisplay.print("V");

  myDisplay.setCursor(84, 16);
  myDisplay.print("T");

  myDisplay.setCursor(124, 16);
  myDisplay.print("H");

  myDisplay.setCursor(164, 16);
  myDisplay.print("WL");

  myDisplay.setCursor(204, 16);
  myDisplay.print("CH4");

  myDisplay.setCursor(244, 16);
  myDisplay.print("MD");

  myDisplay.setCursor(284, 16);
  myDisplay.print("CO");

  //data
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) { //SENSORS PIPES 1..5!
    //sensor num
    myDisplay.setTextSize(2);
    myDisplay.setCursor( 4, (16 + sensorPipeNum * 40));
    if (BASE_sensorIsOn[sensorPipeNum]) {
      myDisplay.setTextColor(ILI9341_GREENYELLOW);
      myDisplay.print(" ");
    } else {
      myDisplay.setTextColor(ILI9341_DARKGREY);
    }
    myDisplay.print(sensorPipeNum, DEC);

    //params
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsAvailable[sensorPipeNum][paramNum]) {
        if (BASE_sensorParamsIsDanger[sensorPipeNum][paramNum]) {
          myDisplay.setTextColor(ILI9341_RED);
        } else {
          myDisplay.setTextColor(ILI9341_GREEN);
        }
        myDisplay.setCursor( (6 + 40 + 40 * paramNum), (16 + 40 * sensorPipeNum) );
        if (abs(BASE_sensorDecodedParams[sensorPipeNum][paramNum] > 99)) {
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
  delay(200);
  gsmSerial.println("AT+CPAS");
  delay(200);
  gsmSerial.println("AT+CSQ");
  delay(200);

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

