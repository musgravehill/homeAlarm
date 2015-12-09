/*
  Note:
  Not all pins on the Mega and Mega 2560 support change interrupts,
  so only the following can be used for RX:
  10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
*/
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("{SD:LOG\t#2\tV3.7\tT23\tH50}");  
  delay(50);
  Serial.print("{SD:ALERT\t#3\tT23}");
  delay(50);
  Serial.print("{GSM:ALERT\t#3\tT23}");
  delay(2000);
}
