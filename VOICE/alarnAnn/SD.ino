void SD_init() {
  if (sd.begin(SD_ChipSelectPin, SPI_FULL_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}
