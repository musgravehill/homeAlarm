#include <avr/wdt.h>

void setup() {
  MCUSR = 0;
  wdt_reset();
  wdt_disable();
  wdt_enable (WDTO_8S);
  Serial.begin(9600);
  Serial.println("Setup..");
  wdt_reset();
  Serial.println(MCUSR); //flags
  Serial.println(WDTCSR);
  Serial.flush();
  wdt_reset();
  Serial.println("Watchdog enabled.");
}

int timer = 0;

void loop() {
  delay(1000);
  timer++;
  Serial.println(timer);
   wdt_reset();
}
