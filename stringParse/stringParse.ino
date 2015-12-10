void setup() {
  Serial.begin(9600);
  String command1 = "LOGS;#2;V3.7;T23;H50" + '\0';
  String command2 = "ALRT;#3;T23" ; // + '\0'; //\0 for strtok, terminated string

  processCommand(command1);
  processCommand(command2);
}

void processCommand(String command) {
  byte minute = 55;
  byte hour = 14;
  byte dayOfMonth = 31;
  byte month = 12;
  byte year = 15;


  String message = command + ";" + hour + ":" +  minute + " " + dayOfMonth + "-" + month + "-" + year;
  String commandType = command.substring(0, 4);
  logSD(message);
  if (commandType == "ALRT") { //LOGS, ALRT
    sendSMS(message, "+79998885533");
  }

  //Serial.println("\r\n IN: " + command);
  //Serial.println(commandType);
}

void sendSMS(String message, String phone) {
  Serial.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);
  Serial.print(message);
  delay(300);
  Serial.print((char)26);
  delay(300);
}

void logSD(String message) {
  Serial.println("SD: " + message);
}

void loop() {

}

