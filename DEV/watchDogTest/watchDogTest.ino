#include <avr/wdt.h>

void setup() {
  wdt_reset();
  wdt_disable();
  Serial.begin(9600);
  Serial.println("Setup..");
  Serial.println("Wait 5 sec..");
  delay(5000);
  wdt_enable (WDTO_8S);
  Serial.println("Watchdog enabled.");
}

int timer = 0;

void loop() {
  delay(1000);
  timer++;
  Serial.println(timer);
  //  wdt_reset();
}
