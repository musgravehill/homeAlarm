void setup() {
  Serial.begin(9600);
  String command1 = "LOGS;#2;V3.7;T23;H50" + '\0';
  String command2 = "ALRT;#3;T23" ; // + '\0'; //\0 for strtok, terminated string

  parseCommand(command1);
  parseCommand(command2);
}

void parseCommand(String command) {
  Serial.println("\r\n IN: " + command);
  String commandType = command.substring(0, 4);
  String commandText = command.substring(5);
  Serial.println(commandType);
  Serial.println(commandText);
}

void loop() {

}

