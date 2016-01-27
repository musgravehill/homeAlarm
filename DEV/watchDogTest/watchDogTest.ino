#include <avr/wdt.h>
#include "LowPower.h"

void setup() {
  MCUSR = 0;  //VERY VERY IMPORTANT!!!! ELSE WDT DOESNTOT RESET, DOESNOT DISABLED!!!
  wdt_disable();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
}

void loop() {
  wdt_enable (WDTO_8S);
  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);
  delay(500);
  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);
  delay(500);
  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);
  delay(7000);

  wdt_reset();
  wdt_disable();

  uint8_t countSleep = 0;
  while (countSleep < 10) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    countSleep++;
  }
}
