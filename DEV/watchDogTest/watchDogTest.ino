#include <avr/wdt.h>

void setup() {
  MCUSR = 0;  //VERY VERY IMPORTANT!!!! ELSE WDT DOESNTOT RESET, DOESNOT DISABLED!!!
  wdt_disable();
  Serial.begin(9600);
  delay(100);
  Serial.println("Setup WDT 8s init");
  wdt_enable (WDTO_8S);
  delay(100);
  wdt_reset();
  pinMode(2, OUTPUT);
}

int timer = 0;

void loop() {
  delay(500);
  digitalWrite(2, LOW);  
  delay(500);
  digitalWrite(2, HIGH);
  timer++;
  Serial.flush();
  Serial.println(timer, DEC);  
}
