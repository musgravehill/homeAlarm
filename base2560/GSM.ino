void GSM_init() {
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
}

void GSM_sendSMS2All(String message) {
  for (uint8_t i = 0; i <= GSM_phoneNums_count; i++) {
    GSM_sendSMS(message, GSM_phoneNums[i]);
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
  delay(50);
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



