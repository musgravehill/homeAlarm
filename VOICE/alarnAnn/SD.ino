void SD_init() {
  if (sd.begin(SD_ChipSelectPin, SPI_HALF_SPEED)) {
    SD_isEnable = true;
  } else {
    SD_isEnable = false;
  }
}

void SD_makeHowtoFile() {
  if (SD_isEnable) {
    // 8.3 filename.ext rule
    if (SD_file.open("howto.txt", O_CREAT | O_TRUNC | O_WRITE)) {
      SD_file.println("=====CREATE FILES IN SD CARD=====");
      SD_file.println("dt.txt    yy.mm.dd.hh.ii.ss.dow.");
      SD_file.println("alarm.txt    hh.ii.");
      SD_file.println("alarm.wav");
      SD_file.println("");
      SD_file.println("=====Using Audacity for creating sound=====");
      SD_file.println("Tracks > Stereo Track to Mono");
      SD_file.println("Project Rate (HZ) > set to 32000, 22050, 16000 or 11025");
      SD_file.println("File > Export > Save as type: Other uncompressed files > Options");
      SD_file.println("Select WAV, Unsigned 8 bit PCM");
      SD_file.println("");
      SD_file.println("https://github.com/TMRh20/TMRpcm");
      SD_file.close();
    }
  }
}
