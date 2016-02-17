void GSM_init() {
  gsmSerial.println("AT"); //synchronize baud rate. GSM by default in auto-baud mode, it will not output any until sync
  delay(200);
  gsmSerial.println("ATE0"); //эхо выключено
  delay(200);
  gsmSerial.println("ATV0"); //возврат кода ответа только 0=OK 4=ERROR
  delay(200);
  gsmSerial.println("AT+CMEE=0"); //если ошибка, то короткий ответ=="ERROR or 4"
  delay(200);
  gsmSerial.println("AT+CLIP=1"); //определитель номера
  delay(200);
  gsmSerial.println("AT+CMGF=1"); //Switching to text mode
  delay(200);
  gsmSerial.println("AT+CSCS=\"GSM\""); //english only
  delay(200);
  //gsmSerial.println("ATS0=1"); //auto-respond to incoming call
  //delay(100);
  gsmSerial.println("AT+CMIC=0,15"); //mic amp
  delay(200);
  gsmSerial.println("AT+ECHO=0,0,0,0,0");
  delay(200);
  gsmSerial.println("AT+SIDET=0,0");
  delay(200);
  gsmSerial.println("AT+SVR=17");
  delay(200);
}

void GSM_ping() {
  gsmSerial.println("AT+CSQ");
  delay(100);
}

void GSM_pingCheckTimeAnswer() {
  if ( (millis() - GSM_prevPingSuccessAnswerMillis) > 8000 ) {
    GSM_reset();
    GSM_prevPingSuccessAnswerMillis = millis();
#ifdef DEBUG
    debugSerial.println("GSM stop responding (hang up) => RST");
#endif
  }
}

void GSM_reset() {
  digitalWrite(GSM_reset_pin, 0);
  delay(300);
  digitalWrite(GSM_reset_pin, 1);
  delay(13000); //init GSM
}

bool GSM_paramIsAllowSms(uint8_t paramNum) {

#ifdef DEBUG
  debugSerial.println(GSM_paramPrevSMSMillis[paramNum], DEC);
#endif

  if (BASE_ALARM_MODE == false) {
#ifdef DEBUG
    debugSerial.println("BASE_ALARM_MODE == false");
#endif
    return false;
  }
  if (GSM_paramPrevSMSMillis[paramNum] == 1) {
#ifdef DEBUG
    debugSerial.println("== 1");
#endif
    return true;
  }
  if ( (millis() - GSM_paramPrevSMSMillis[paramNum]) > GSM_periodParamAllowSMSMillis[paramNum] ) {
#ifdef DEBUG
    debugSerial.println("millis");
#endif
    GSM_paramPrevSMSMillis[paramNum] = millis();
    return true;
  }
  else {
#ifdef DEBUG
    debugSerial.println("def false");
#endif
    return false;
  }
}

void GSM_initSmsDangers() {
  String SMS_danger = "";
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) {
    for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
      if (BASE_sensorParamsIsDanger[sensorPipeNum][paramNum] && GSM_paramIsAllowSms(paramNum)) {
        SMS_danger =  "#" + String(sensorPipeNum, DEC) + " ";
        SMS_danger +=  PARAMS_getVerbalParamName(paramNum) + "=";
        SMS_danger +=  String(BASE_sensorDecodedParams[sensorPipeNum][paramNum], DEC) + " ";
        GSM_addToQueueSMS_forAllPhones(SMS_danger);

      }
    }
  }
}

void GSM_addToQueueSMS_forAllPhones(String message) {
  for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
    GSM_queueLoop_phones[GSM_queueLoop_pos] = GSM_phoneNums[i]; //0..7
    GSM_queueLoop_messages[GSM_queueLoop_pos] = message.substring(0, 63);
    GSM_queueLoop_pos++; //create next pos
    if (GSM_queueLoop_pos > (GSM_queueLoop_size - 1)) {
      GSM_queueLoop_pos = 0; //loop, goto 0 position in queue
    }
  }
}

void GSM_queueLoopSMS_stateMachine_processing() {
  //check if not null
  if (GSM_queueLoop_phones[GSM_queueLoop_stateMachine_pos] != "") {
    GSM_sendSMS(GSM_queueLoop_phones[GSM_queueLoop_stateMachine_pos], GSM_queueLoop_messages[GSM_queueLoop_stateMachine_pos]);
#ifdef DEBUG
    debugSerial.print("SMS WAS SENT:");
    debugSerial.print(GSM_queueLoop_phones[GSM_queueLoop_stateMachine_pos] + " ");
    debugSerial.println(GSM_queueLoop_messages[GSM_queueLoop_stateMachine_pos]);
#endif
    GSM_queueLoop_phones[GSM_queueLoop_stateMachine_pos] = "";
    GSM_queueLoop_messages[GSM_queueLoop_stateMachine_pos] = "";
  }
  GSM_queueLoop_stateMachine_pos++; //create next pos
  if (GSM_queueLoop_stateMachine_pos > (GSM_queueLoop_size - 1)) {
    GSM_queueLoop_stateMachine_pos = 0; //loop, goto 0 position in queue
  }
}

void GSM_sendSMS(String phone, String message) {
  gsmSerial.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  gsmSerial.print(message);
  delay(50);
  gsmSerial.print((char)26);
  delay(50);
  //delay in state machine
}

void GSM_cleanAllSMS() {
  gsmSerial.println("AT+CMGD=1,4"); //clean ALL SMS (in, out, read, unread, sent, unsent)
  delay(1000);
}

void GSM_initPhoneNums() {
  //+xxxxxxxxxx,+xxxxxxxxxx,+xxxxxxxxxx
  GSM_phoneNums_count = 0;
  if (SD_isEnable) {
    if (SD_file.open("phones.txt", O_READ)) { //8.3 filename.ext rule
      uint8_t i = 0;
      char chr;
      GSM_phoneNums[i] = "";
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
          //GSM_phoneNums[i] += chr;
          GSM_phoneNums[i] += String((char)chr);
        }
        if (chr == ',') {
          GSM_phoneNums_count++;
          i++;
          GSM_phoneNums[i] = "";
        }
      }
      SD_file.close();
      GSM_phoneNums_count++;
    }
  }

#ifdef DEBUG
  debugSerial.print("GSM_phoneNums_count=");
  debugSerial.println(GSM_phoneNums_count, DEC);
  for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
    debugSerial.print(i, DEC);
    debugSerial.print("___");
    debugSerial.print(GSM_phoneNums[i]);
    debugSerial.println("___");
  }
#endif

}

void GSM_listenSerial() {
  String strGsm = "";
  uint8_t strGsmLength = 0;
  while (gsmSerial.available() && (strGsmLength <= 64) ) {
    char charFromGSM = gsmSerial.read();
    strGsmLength++;
    if (charFromGSM == '\r') { //end of text
      GSM_processSerialString(strGsm);
      strGsm = "";
      strGsmLength = 0;
    }
    else if (charFromGSM == '\n') {
      strGsm = "";
      strGsmLength = 0;
    }
    else {
      strGsm += (char)charFromGSM;
    }
  }
}

void GSM_checkIncomingCall() {
  String phoneIncoming = "";
  if (GSM_answerCLIP.length() > 20) {  //+CLIP: "+7915977xxxx",145,"",0,"",0\r\n
    phoneIncoming = GSM_answerCLIP.substring(8, 20); //+CLIP: "+7915977xxxx   //sub [from, until)
    for (uint8_t i = 0; i < GSM_phoneNums_count; i++) {
      if ( phoneIncoming  == GSM_phoneNums[i] ) {
        gsmSerial.println("ATA");// respond to incoming call
      }
      SD_logIncomingCall(phoneIncoming);
#ifdef DEBUG
      debugSerial.println("_" + phoneIncoming + "=?=" + GSM_phoneNums[i] + "_");
#endif
    }
#ifdef DEBUG
    debugSerial.println("CLIP_NUM:" + phoneIncoming);
#endif
  }
  GSM_answerCLIP = ""; //DONT SEND "ATA" AGAIN!
}

void GSM_processSerialString(String s) {
  if (s.length() > 5) {
    String s_head = s.substring(0, 5); //+CSQ: //+CLIP
    if (s_head == "+CSQ:") {
      GSM_answerCSQ = s;
    }
    else if (s_head == "+CLIP") {
      GSM_answerCLIP = s;
    }
    else if (s_head == "+CPAS") {
      GSM_answerCPAS = s;
    }
    else if (s_head == "+COPS") {
      GSM_answerCOPS = s;
    }
    GSM_prevPingSuccessAnswerMillis = millis(); //if too long -> GSM RESET
    
    //#ifdef DEBUG
    //debugSerial.println("gsm_head:" + s_head);
    //#endif
  }
  //#ifdef DEBUG
  // debugSerial.println("GSM:" + s);
  //#endif
}

