int sensorPin = A1;
int sensorValue = 0;

void setup() {
  Serial.begin(57600);
  pinMode(2, OUTPUT);
}

void loop() {
  //sensorValue = analogRead(sensorPin);
  //Serial.println(sensorValue, DEC);
  delay(50);
  digitalWrite(2, HIGH);
  delay(50);
  digitalWrite(2, LOW);
}
