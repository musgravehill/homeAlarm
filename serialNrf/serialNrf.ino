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
  Serial.print("{LOG=#2;V3.7;T23;H50}");  
  delay(50);
  Serial.print("{ALERT=#3;T23}");
  delay(50);
  Serial.print("{ALERT=#3;T23}");
  delay(2000);
}
