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

  //clean ALL SMS (in, out, read, unread, sent, unsent)
  gsmSerial.println("AT+CMGD=1,4");
  delay(300);
}
