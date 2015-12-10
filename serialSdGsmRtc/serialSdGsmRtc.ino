#include <SdFat.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Wire.h"

#define DS3231_I2C_ADDRESS 0x68

const byte SD_CS =  4; //SS
SdFat SD_card;
File SD_file_log;
SdFile SD_isEnable = false;

SoftwareSerial serialCommand(7, 8); // RX, TX
String command;

void setup() {
  delay(1000);
  pinMode(SD_CS, OUTPUT);
  pinMode(10, OUTPUT); //hardware ATmega SPI SS
  delay(100);
  Serial.begin(9600);
  delay(100);
  serialCommand.begin(9600);
  delay(100);
  SD_init();
  delay(100);
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,42,21,4,26,11,14);
  delay(100);
}

void loop() { // run over and over
  bool isEndOfCommand = false;
  char getByte;
  while (serialCommand.available() & !isEndOfCommand ) {
    char getByte = serialCommand.read();
    if (getByte == '{') {
      command = ""; //new command start, clean buffer (?)
    }
    if (getByte == '}') {
      isEndOfCommand = true;
      processCommand(command);
      command = "";
    }
    else {
      command += getByte;
    }
  }
}

void processCommand(String command) {
  Serial.println(command);
  SD_log(command);
}

void SD_log(String command) {
  if (SD_isEnable) {
    SD_file_log = SD_card.open("log.txt", O_WRITE | O_CREAT | O_APPEND); // 8.3 filename.ext rule
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


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
                    byte *minute,
                    byte *hour,
                    byte *dayOfWeek,
                    byte *dayOfMonth,
                    byte *month,
                    byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                 &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second < 10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch (dayOfWeek) {
    case 1:
      Serial.println("Sunday");
      break;
    case 2:
      Serial.println("Monday");
      break;
    case 3:
      Serial.println("Tuesday");
      break;
    case 4:
      Serial.println("Wednesday");
      break;
    case 5:
      Serial.println("Thursday");
      break;
    case 6:
      Serial.println("Friday");
      break;
    case 7:
      Serial.println("Saturday");
      break;
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

