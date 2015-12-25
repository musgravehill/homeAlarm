#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>  //48*84
// Software SPI (slower updates, more flexible pin options):
// pin 6 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 4 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 2 - LCD reset (RST)
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(6, 5, 4, 3, 2);

void setup() {
  delay(100);
  myDisplay.begin();
  myDisplay.setContrast(60);
  myDisplay.clearDisplay();
  myDisplay.setRotation(0);
}

void loop() {
  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.println("{LOG:BOB:ANNA;H:5655;T:245;C:042;V:05}{DNGR:ANNA:LOVE}{0123456789abcdefghijklmnopqrstuvwxyz}");
  myDisplay.display();

  myDisplay.fillRect(0, 39, 84, 9, 0);//x0, y0, w, h, color

  for (int i = 0; i < 5; i++) {
    if (random(0,2) == 1) { //2 hours
      //sensor fault
      myDisplay.fillRect(i*12, 39, 9, 9, 1);//x0, y0, w, h, color
    }
    else {
      //sensor ok
      myDisplay.fillRect(i*13, 40, 8, 8, 1);//x0, y0, w, h, color
      myDisplay.fillRect(i*13+1, 41, 6, 6, 0);//x0, y0, w, h, color
    }

  }
  myDisplay.display();
  delay(2000);
}






