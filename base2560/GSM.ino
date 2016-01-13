void GSM_init() {
  gsmSerial.println("AT"); //synchronize baud rate. GSM by default in auto-baud mode, it will not output any until sync
  delay(100);
  gsmSerial.println("ATE0"); //эхо выключено
  delay(100);
  gsmSerial.println("ATV0"); //возврат кода ответа только 0=OK 4=ERROR
  delay(100);
  gsmSerial.println("AT+CMEE=0"); //если ошибка, то короткий ответ=="ERROR or 4"
  delay(100);
  gsmSerial.println("AT+CLIP=1"); //определитель номера
  delay(100);
  gsmSerial.println("AT+CMGF=1"); //Switching to text mode
  delay(100);
  gsmSerial.println("AT+CSCS=\"GSM\""); //english only
  delay(100);
  //gsmSerial.println("ATS0=1"); //auto-respond to incoming call
  //delay(100);
  gsmSerial.println("AT+CMIC=0,15"); //mic amp
}

bool GSM_paramIsAllowSms(uint8_t paramNum) {
  if (GSM_paramPrevSMSMillis[paramNum] == 0) {
    return true;
  }
  if ( (millis() - GSM_paramPrevSMSMillis[paramNum]) >  GSM_periodParamAllowSMSMillis[paramNum]) {
    return true;
  }
  else {
    return false;
  }
}

void GSM_sendDangers() {
  String SMS_dangers = "";
  bool isAllowSendSMS = false;
  /*
    Все опасные параметры от всех датчиков пишем в строку.
    Если можно отправить СМС хотя бы для 1 параметра, то щлем всю строку.
  */
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) {
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsDanger[sensorPipeNum][paramNum]) {
        SMS_dangers +=  "#" + String(sensorPipeNum, DEC) + ":";
        SMS_dangers +=  PARAMS_getVerbalParamName(paramNum) + "=";
        SMS_dangers +=  String(BASE_sensorDecodedParams[sensorPipeNum][paramNum], DEC) + ";";
        if (GSM_paramIsAllowSms(paramNum)) {
          isAllowSendSMS = true;
          GSM_paramPrevSMSMillis[paramNum] = millis();
        }
      }
    }
  }

  if (isAllowSendSMS) {
    GSM_sendSMS2All(SMS_dangers);
  }

#ifdef DEBUG
  debugSerial.print("SMS DNGR++: ");
  debugSerial.println(SMS_dangers);
#endif

}

void GSM_sendSMS2All(String message) {
#ifdef DEBUG
  debugSerial.println("SMS DNGR SEND:");
  debugSerial.println(message);
#endif
  for (uint8_t i = 0; i <= GSM_phoneNums_count; i++) {
    GSM_sendSMS(message, GSM_phoneNums[i]);
#ifdef DEBUG
    debugSerial.println(GSM_phoneNums[i]);
#endif
  }
}

void GSM_sendSMS(String message, String phone) {
  //TODO delete delay, use state machine
  gsmSerial.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.print((char)26);
  delay(1000);
}

void GSM_cleanAllSMS() {
  gsmSerial.println("AT+CMGD=1,4"); //clean ALL SMS (in, out, read, unread, sent, unsent)
  delay(300);
}

void GSM_initPhoneNums() {
  //+xxxxxxxxxx,+xxxxxxxxxx,+xxxxxxxxxx
  GSM_phoneNums_count = 0;
  if (SD_isEnable) {
    if (SD_file.open("phones.txt", O_READ)) { //8.3 filename.ext rule
      uint8_t i = 0;
      char chr;
      while (SD_file.available()) {
        chr = SD_file.read();
        if ( chr == '0'
             || chr == '1'
             || chr == '2'
             || chr == '3'
             || chr == '4'
             || chr == '5'
             || chr == '6'
             || chr == '7'
             || chr == '8'
             || chr == '9'
             || chr == '+' )
        {
          GSM_phoneNums[i] += chr;
          //GSM_phoneNums[i] += String(chr);
        }
        if (chr == ',') {
          GSM_phoneNums_count++;
          i++;
        }
      }
      SD_file.close();
    }
  }
}

void GSM_answerCall() {
  String stringFromGSM = "";     //+CLIP: "+7915977xxxx",145,"",0,"",0
  String s = "";
  bool isAllowRespond = false;
  while (gsmSerial.available()) {
    char charFromGSM = gsmSerial.read();
    if (charFromGSM == '+') {
      stringFromGSM = "+";
    }
    if (
      charFromGSM == '0'
      || charFromGSM == '1'
      || charFromGSM == '2'
      || charFromGSM == '3'
      || charFromGSM == '4'
      || charFromGSM == '5'
      || charFromGSM == '6'
      || charFromGSM == '7'
      || charFromGSM == '8'
      || charFromGSM == '9'
    ) {
      stringFromGSM += String((char)charFromGSM);
    }

    if (charFromGSM == '"') {
      for (uint8_t i = 0; i <= GSM_phoneNums_count; i++) {
#ifdef DEBUG
        s = "_" + String(stringFromGSM); // + "_==_" + String(GSM_phoneNums[i]) + "_";
        debugSerial.println(s);
#endif
        if (stringFromGSM == GSM_phoneNums[i]) {
          isAllowRespond = true;
        }
      }
      stringFromGSM = "";
    }

    if (stringFromGSM.length() > 64) {
      stringFromGSM = "";
    }
  }

  if (isAllowRespond) {
    gsmSerial.println("ATA");// respond to call
  }
}
