#include <SdFat.h>
#include <SPI.h>
#include <SoftwareSerial.h>

const byte SD_CS =  4; //SS
SdFat SD_card;
File SD_file_log;
bool SD_isEnable = false;

SoftwareSerial serialCommand(7, 8); // RX, TX
String command;

void setup() {
  Serial.begin(9600);
  serialCommand.begin(9600);

  pinMode(SD_CS, OUTPUT);
  pinMode(10, OUTPUT); //hardware ATmega SPI SS

  SD_init();
}

void loop() { // run over and over
  bool isEndOfCommand = false;
  char getByte;
  while (serialCommand.available() & !isEndOfCommand ) {
    char getByte = serialCommand.read();
    command += getByte;
    if (getByte == '{') {
      command = "{"; //new command start, delete old (?)
    }
    if (getByte == '}') {
      isEndOfCommand = true;
      processCommand(command);
      command = "";
    }
  }
}

void processCommand(String command) {
  Serial.print(command);
  SD_log(command);
}

void SD_log(String command) {
  if (SD_isEnable) {
    SD_file_log = SD_card.open("log.txt", FILE_WRITE); // 8.3 filename.ext rule
    if (SD_file_log) {
      SD_file_log.println(command);
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

