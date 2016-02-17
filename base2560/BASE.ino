void BASE_processDataFromSensor() {
  millisPrevSignal_sensors[NRF_currPipeNum] =  millis(); //save time of sensor answer
  String string_logs = "LOGS;#" + String(NRF_currPipeNum, DEC) + ";";
  String string_dangers = "DNGR;#" + String(NRF_currPipeNum, DEC) + ";";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  int16_t paramVal_decoded;
  bool issetDangers = false;

  DateTime now = RTC3231.now();
  uint8_t hh =  now.hour();
  uint8_t ii =  now.minute();
  String hhii = ((hh < 10) ? "0" : "") + String(hh, DEC) + ":" ;
  hhii += ((ii < 10) ? "0" : "") + String(ii, DEC);

  for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
    if (NRF_messageFromSensor[paramNum] > 0) { //param is available
      paramVal_decoded = PARAMS_decodeParam(paramNum, NRF_messageFromSensor[paramNum]); //decode to real range
      string_logs +=  String((char)paramCode[paramNum]) + ";" + String(paramVal_decoded, DEC) + ";";
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = true;
      BASE_sensorDecodedParams[NRF_currPipeNum][paramNum] = paramVal_decoded;

      //param is danger
      if (PARAMS_isDangerParamValue(paramNum, paramVal_decoded)) {
        issetDangers = true;
        string_dangers += String((char)paramCode[paramNum]) + ";";
        string_dangers += String(paramVal_decoded, DEC) + ";";
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = true;
      }
      else {
        string_dangers += String((char)paramCode[paramNum]) + ";;";
        BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = false;
      }
    }
    //param NOT available
    else {
      string_logs += String((char)paramCode[paramNum]) +  ";;";
      string_dangers += String((char)paramCode[paramNum]) + ";;";
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = false;
    }
  }

  string_logs += hhii + ";";
  SD_log(string_logs);
  if (issetDangers) {
    string_dangers += hhii + ";";
    SD_log(string_dangers);
  }

  BASE_setAlarmMode();

#ifdef DEBUG
  debugSerial.println(string_logs);
  debugSerial.println(string_dangers);
#endif
}

void BASE_setAlarmMode() {
  if (NRF_messageFromSensor[7] > 0) { //param7 = button "alarm on\off" on sensor box
    //BASE_ALARM_MODE = (101 == NRF_messageFromSensor[7]) ? true : false;

    DateTime now = RTC3231.now();
    uint8_t hh =  now.hour();
    uint8_t ii =  now.minute();
    String hhii = ((hh < 10) ? "0" : "") + String(hh, DEC) + ":" ;
    hhii += ((ii < 10) ? "0" : "") + String(ii, DEC);

    String string_alarmMode = "ALRMMODE;#" + String(NRF_currPipeNum, DEC) + ";";
    string_alarmMode += String((char)(BASE_ALARM_MODE) ? "TRUE" : "FALSE") + ";";
    string_alarmMode += hhii + ";";
    SD_log(string_alarmMode);
#ifdef DEBUG
    debugSerial.println(string_alarmMode);
#endif
  }
}

void BASE_checkSensorsFault() {
  uint32_t millisCurr = millis();
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) { //SENSORS PIPES 1..5!
    if ((millisCurr - millisPrevSignal_sensors[sensorPipeNum]) > BASE_sensorSilenceFaultMillis) {
      BASE_sensorIsOn[sensorPipeNum] = false; //sensor fault
    }
    else {
      BASE_sensorIsOn[sensorPipeNum] = true; //sensor ok
    }
  }
}

void wr() {
  wdt_reset();
}
