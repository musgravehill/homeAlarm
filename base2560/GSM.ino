void GSM_init() {
  gsmSerial.println("AT+CLIP=1"); //turn on caller ID notification
  delay(100);
  gsmSerial.println("AT+CMGF=1"); //Switching to text mode
  delay(100);
  gsmSerial.println("AT+CSCS=\"GSM\""); //english only
  delay(100);
}

void GSM_sendSMS(String message, String phone) {
  gsmSerial.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  gsmSerial.print(message);
  delay(300);
  gsmSerial.print((char)26);
  delay(300);
}

void GSM_cleanAllSMS() {
  gsmSerial.println("AT+CMGD=1,4"); //clean ALL SMS (in, out, read, unread, sent, unsent)
  delay(50);
}

void GSM_initPhoneNums() {
  GSM_phoneNums_count = 0;
  if (SD_isEnable) {
    if (SD_file.open("phones.txt", O_READ)) { //8.3 filename.ext rule
      uint8_t i = 0;
      byte chr;
      while ((chr = SD_file.read()) >= 0) {
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
        }
        if (chr == '\n') {
          GSM_phoneNums_count++;
          i++;
        }
      }
      SD_file.close();
      GSM_phoneNums_count--; //cause last /r/n increase size +1
    }
  }
}



