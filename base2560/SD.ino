void SD_init() {
  if (SD_card.begin(SD_CS, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void SD_log(String data) {
  if (SD_isEnable) {

    DateTime now = RTC3231.now();
    //uint32_t nowUt = now.unixtime();
    uint16_t yy =  now.year() - 2000;
    uint8_t mm = now.month();
    uint8_t dd =  now.day();

    String filename_s = ((dd < 10) ? "0" : "") + String(dd, DEC) + ".";
    filename_s +=  ((mm < 10) ? "0" : "") + String(mm, DEC) + ".";
    filename_s += ((yy < 10) ? "0" : "") + String(yy, DEC);
    filename_s += ".csv";
    char filename_chr[filename_s.length() + 1];
    filename_s.toCharArray(filename_chr, sizeof(filename_chr));

    // 8.3 filename.ext rule
    if (SD_file.open( filename_chr, O_WRITE | O_CREAT | O_APPEND)) {
      SD_file.println(data);
      SD_file.close();
    }
  }
  delay(500);
}

void SD_logIncomingCall(String phone) {
  if (SD_isEnable) {
    DateTime now = RTC3231.now();
    //uint32_t nowUt = now.unixtime();
    uint16_t yy =  now.year() - 2000;
    uint8_t mm = now.month();
    uint8_t dd =  now.day();
    uint8_t hh =  now.hour();
    uint8_t ii =  now.minute();

    String filename_s = "incall.csv";
    char filename_chr[filename_s.length() + 1];
    filename_s.toCharArray(filename_chr, sizeof(filename_chr));
    // 8.3 filename.ext rule
    if (SD_file.open( filename_chr, O_WRITE | O_CREAT | O_APPEND)) {
      String data = phone + ";";
      data += ((dd < 10) ? "0" : "") + String(dd, DEC) + ".";
      data += ((mm < 10) ? "0" : "") + String(mm, DEC) + ".";
      data += ((yy < 10) ? "0" : "") + String(yy, DEC) + " ";
      data += ((hh < 10) ? "0" : "") + String(hh, DEC) + ":";
      data += ((ii < 10) ? "0" : "") + String(ii, DEC) + ";";

      SD_file.println(data);
      SD_file.close();
    }
  }
  delay(500);
}

