void setup() {
  Serial.begin(9600);
  String command1 = "LOGS;#2;V3.7;T23;H50";
  String command2 = "ALRT;#5;H70"; // + '\0'; //\0 for strtok, terminated string
  /String command3 = "DNGR;#1;W1";

  processCommand(command1);
  processCommand(command2);
  processCommand(command3);
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
  if (commandType == "DNGR") { //LOGS, ALRT
    sendSMS(message, "+79998885533");
  }

  //Serial.println("\r\n IN: " + command);
  //Serial.println(commandType);
}



void logSD(String message) {
  Serial.println("SD: " + message);
}

void loop() {

}

