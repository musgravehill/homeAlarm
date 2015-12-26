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
const int latchPin = A0;
//Пин подключен к SH_CP входу 74HC595
const int clockPin = A1;
//Пин подключен к DS входу 74HC595
const int dataPin = A2;

void setup() {
  delay(100);
  myDisplay.begin();
  myDisplay.setContrast(60);
  myDisplay.clearDisplay();
  myDisplay.setRotation(0);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {
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
    }
    else {
      //sensor ok

    }

  }
  myDisplay.display();
  delay(1000);

  // проходим циклом по всем 16 выходам двух регистров
  for (int thisLed = 0; thisLed < 16; thisLed++) {
    // записываем сигнал в регистр для очередного светодиода
    registerWrite(thisLed, HIGH);
    // если это не первый светодиод, то отключаем предыдущий
    if (thisLed > 0) {
      registerWrite(thisLed - 1, LOW);
    }
    // если это первый светодиод, то отключаем последний
    else {
      registerWrite(15, LOW);
    }
    delay(100);
  }
}

// этот метод отсылает бит на сдвиговый регистр
void registerWrite(int whichPin, int whichState) {
  // для хранения 16 битов используем unsigned int
  unsigned int bitsToSend = 0;

  // выключаем светодиоды на время передачи битов
  digitalWrite(latchPin, LOW);

  // устанавливаем HIGH в соответствующий бит
  bitWrite(bitsToSend, whichPin, whichState);

  // разбиваем наши 16 бит на два байта
  // для записи в первый и второй регистр
  byte registerOne = highByte(bitsToSend);
  byte registerTwo = lowByte(bitsToSend);

  // "проталкиваем" байты в регистры
  shiftOut(dataPin, clockPin, MSBFIRST, registerTwo);
  shiftOut(dataPin, clockPin, MSBFIRST, registerOne);

  // "защелкиваем" регистр, чтобы биты появились на выходах регистра
  digitalWrite(latchPin, HIGH);
}







