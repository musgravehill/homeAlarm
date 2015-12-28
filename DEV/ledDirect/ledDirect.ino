//Пин подключен к ST_CP входу 74HC595
const int LED_latchPin = A0;
//Пин подключен к SH_CP входу 74HC595
const int LED_clockPin = A1;
//Пин подключен к DS входу 74HC595
const int LED_dataPin = A2;

uint16_t LED_twoBytes;
byte LED_twoByte1;
byte LED_twoByte2;

void setup() {
  Serial.begin(9600);

  pinMode(LED_latchPin, OUTPUT);
  pinMode(LED_dataPin, OUTPUT);
  pinMode(LED_clockPin, OUTPUT);

  LED_twoBytes = 0b0;
  bitWrite(LED_twoBytes, 0, 1);
  bitWrite(LED_twoBytes, 9, 1);
  bitWrite(LED_twoBytes, 15, 1);
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready

}

void loop() {
  int val = analogRead(A5);

  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoBytes = 0b0;
  if (val > 230) {
    bitWrite(LED_twoBytes, 0, 0);
    bitWrite(LED_twoBytes, 1, 1);
  }
  else {
    bitWrite(LED_twoBytes, 0, 1);
    bitWrite(LED_twoBytes, 1, 0);
  }
  
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  Serial.println(val, DEC);
  delay(500);
}
