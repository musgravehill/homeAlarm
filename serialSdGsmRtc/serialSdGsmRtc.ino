#include <SdFat.h>
#include <SPI.h>

const byte SD_CS =  4; //SS
SdFat SD_card;
File SD_file_log;
bool SD_isEnable = false;

String command;

void setup() {
  Serial.begin(9600);

  pinMode(SD_CS, OUTPUT);
  pinMode(10, OUTPUT); //hardware ATmega SPI SS

  SD_init();
}

void loop() { // run over and over 
  bool isEndOfCommand = false;
  char getByte;
  while (Serial.available() & !isEndOfCommand ) {
    char getByte = Serial.read();
    command+= getByte;      
    if (getByte == '}') {
      isEndOfCommand = true;
      Serial.println(command);
      processCommand(command);
      command = "";
    }
  }
}

void processCommand(char* command) {
  String c = String(command);
  Serial.println(c);
  if (SD_isEnable) {
    SD_file_log = SD_card.open("events.txt", FILE_WRITE);
    if (SD_file_log) {
      SD_file_log.println(c);
      SD_file_log.close();
    }
  }
}

void SD_init() {
  if (SD_card.begin(SD_CS, SPI_HALF_SPEED)) {
    SD_isEnable = true;
    Serial.println("SD init OK");
  } else {
    SD_isEnable = false;
    Serial.println("SD init ERROR");
  }
}

/*
   String str;

  void loop()
  {
    if(Serial.available() > 0)
    {
        str = Serial.readStringUntil('\n');
        x = Serial.parseInt();
    }
  }
*/

