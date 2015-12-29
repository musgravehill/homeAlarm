void SD_init() {
  if (SD_card.begin(SD_CS, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void SD_log(String data) {
  if (SD_isEnable) {
    /*
      DateTime now = RTC3231.now();
      uint32_t nowUt = now.unixtime();
      uint16_t yyyy =  now.year();
      uint8_t mm = now.month();
      uint8_t dd =  now.day();
    */
    uint16_t yy =  2015 - 2000;
    uint8_t mm = 12;
    uint8_t dd = 28;
    String filename_s = String(dd, DEC) + "." + String(mm, DEC) + "." + String(yy, DEC) + ".csv";
    char filename_chr[filename_s.length() + 1];
    filename_s.toCharArray(filename_chr, sizeof(filename_chr));

    // 8.3 filename.ext rule
    if (SD_file.open( filename_chr, O_WRITE | O_CREAT | O_APPEND)) {
      SD_file.println(data);
      SD_file.close();
    }
  }
}
