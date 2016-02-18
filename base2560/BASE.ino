void BASE_processDataFromSensor() {
  millisPrevSignal_sensors[NRF_currPipeNum] =  (uint32_t) millis(); //save time of sensor answer
  int16_t paramVal_decoded;

  for (uint8_t paramNum = 0; paramNum < 7; paramNum++) {
    if (NRF_messageFromSensor[paramNum] > 0) { //param is available
      paramVal_decoded = PARAMS_decodeParam(paramNum, NRF_messageFromSensor[paramNum]);
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = (bool) true;
      BASE_sensorDecodedParams[NRF_currPipeNum][paramNum] = (int16_t) paramVal_decoded;
      BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = (bool) PARAMS_isDangerParamValue(paramNum, paramVal_decoded);
    }
    else {  //param NOT available
      BASE_sensorParamsIsAvailable[NRF_currPipeNum][paramNum] = (bool) false;
      BASE_sensorParamsIsDanger[NRF_currPipeNum][paramNum] = (bool) false;
      BASE_sensorDecodedParams[NRF_currPipeNum][paramNum] = (int16_t) 0;
    }
  }
  SD_sensorParamsLog(NRF_currPipeNum);
}

void BASE_checkSensorsFault() {
  uint32_t millisCurr = millis();
  for (uint8_t sensorPipeNum = 1; sensorPipeNum < 6; sensorPipeNum++) { //SENSORS PIPES 1..5!
    if ((millisCurr - millisPrevSignal_sensors[sensorPipeNum]) > BASE_sensorSilenceFaultMillis) {
      BASE_sensorIsOn[sensorPipeNum] = (bool) false; //sensor fault
    }
    else {
      BASE_sensorIsOn[sensorPipeNum] = (bool) true; //sensor ok
    }
  }
}

void wr() {
  wdt_reset();
}
