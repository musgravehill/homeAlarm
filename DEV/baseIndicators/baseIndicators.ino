#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>  //48*84
// Software SPI (slower updates, more flexible pin options):
// pin 6 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 4 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 2 - LCD reset (RST)
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(6, 5, 4, 3, 2);

//Пин подключен к ST_CP входу 74HC595
const int LED_latchPin = A0;
//Пин подключен к SH_CP входу 74HC595
const int LED_clockPin = A1;
//Пин подключен к DS входу 74HC595
const int LED_dataPin = A2;

uint16_t LED_twoBytes = B0;

void setup() {
  delay(100);
  myDisplay.begin();
  myDisplay.setContrast(60);
  myDisplay.clearDisplay();
  myDisplay.setRotation(0);

  pinMode(LED_latchPin, OUTPUT);
  pinMode(LED_dataPin, OUTPUT);
  pinMode(LED_clockPin, OUTPUT);
}

void loop() {
  LED_twoBytes = B0;

  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.println("{LOG:BOB:ANNA;H:5655;T:245;C:042;V:05}{DNGR:ANNA:LOVE}{0123456789abcdefghijklmnopqrstuvwxyz}");
  myDisplay.display();

  myDisplay.fillRect(0, 40, 84, 8, 0);//x0, y0, w, h, color
  for (int i = 0; i < 5; i++) {
    myDisplay.fillRect(i * 12, 40, 8, 8, 1); //x0, y0, w, h, color
    if (random(0, 2) == 1) { //2 hours
      //sensor fault
      myDisplay.fillRect((i * 12 + 1), 41, 6, 6, 0); //x0, y0, w, h, color
      bitWrite(LED_twoBytes, i, 1); // 1st 74HC595 with red
      //LED_twoBytes|= 10;
    }
    else {
      //sensor ok
      bitWrite(LED_twoBytes, (i + 8), 1); //+8 => 2th 74HC595 with green
    }

  }
  myDisplay.display();

  digitalWrite(LED_latchPin, LOW); //leds off
  byte LED_twoByte1 = highByte(LED_twoBytes);
  byte LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(4000);

}

