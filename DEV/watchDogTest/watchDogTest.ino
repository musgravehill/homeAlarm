#include <avr/wdt.h>

void setup() {
  wdt_disable(); // ����������� ������ �� ������� �� ������� ���������� ��� bootloop
  Serial.begin(9600);
  Serial.println("Setup..");
  
  Serial.println("Wait 5 sec..");
  delay(5000); // ��������, ����� ���� ����� ����������� ���������� � ������ bootloop
  wdt_enable (WDTO_8S); // ��� ������ �� ������������� ������������� �������� ����� 8 ���.
  Serial.println("Watchdog enabled.");
}

int timer = 0;

void loop(){
  // ������ ������� ������ ����������� � �������� �������� ����� � Serial
  if(!(millis()%1000)){
    timer++;
    Serial.println(timer);
    digitalWrite(13, digitalRead(13)==1?0:1); delay(1);
  }
//  wdt_reset();
}
