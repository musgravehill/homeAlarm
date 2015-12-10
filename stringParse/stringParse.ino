void setup() {
  Serial.begin(9600);
  String command = "LOGS=#2;V3.7;T23;H50" ; // + '\0';
  String command2 = "ALRT=#3;T23" ; // + '\0'; //\0 for strtok, terminated string

  String commandType = command.substring(0,4);
  Serial.println(commandType);
}

void loop() {

}
