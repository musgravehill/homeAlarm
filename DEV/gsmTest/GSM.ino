void GSM_init() {
  gsmSerial.println("ATE0"); //эхо выключено
  delay(100);
  gsmSerial.println("ATV0"); //возврат кода ответа только 0=OK 4=ERROR
  delay(100);
  gsmSerial.println("AT+CMEE=0"); //если ошибка, то ответ=="ERROR or 4" только
  delay(100);
  gsmSerial.println("AT+CLIP=1"); //определитель номера
  delay(100);
  gsmSerial.println("AT+CMGF=1"); //Switching to text mode
  delay(100);
  gsmSerial.println("AT+CSCS=\"GSM\""); //english only
  delay(100);
}

void GSM_sendSMS(String message, String phone) {
  gsmSerial.println("AT+CMGS=\"" + phone + "\"");
  delay(100);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.print((char)26);
  delay(1000);
}

void GSM_cleanAllSMS() {
  gsmSerial.println("AT+CMGD=1,4"); //clean ALL SMS (in, out, read, unread, sent, unsent)
  delay(50);
}



