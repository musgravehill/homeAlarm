//Пин подключен к ST_CP входу 74HC595
const int LED_latchPin = A0;
//Пин подключен к SH_CP входу 74HC595
const int LED_clockPin = A1;
//Пин подключен к DS входу 74HC595
const int LED_dataPin = A2;

uint16_t LED_twoBytes = B0;
byte LED_twoByte1;
byte LED_twoByte2;

void setup() {
  pinMode(LED_latchPin, OUTPUT);
  pinMode(LED_dataPin, OUTPUT);
  pinMode(LED_clockPin, OUTPUT);

  LED_twoBytes = 0b0;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);

  LED_twoBytes = 0b1111111111111111;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);

  LED_twoBytes = 0b1111111100000000;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);

  LED_twoBytes = 0b11111111;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);

  LED_twoBytes = 0b1010101010101010;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);

  LED_twoBytes = 0b0101010101010101;
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(1000);
}

void loop() {
  LED_twoBytes = random(0, 65535);
  digitalWrite(LED_latchPin, LOW); //leds off
  LED_twoByte1 = highByte(LED_twoBytes);
  LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(200);
}
