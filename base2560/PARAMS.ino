String PARAMS_getVerbalParamName(String systemParamName) {
  if (systemParamName == "V") {
    return F("_VOLTAGE_");
  }
  if (systemParamName == "T") {
    return F("_TEMPERATURE_");
  }
  if (systemParamName == "H") {
    return F("_HUMIDITY_");
  }
  if (systemParamName == "W") {
    return F("_WATER_LEAK_");
  }
  if (systemParamName == "G") {
    return F("_GAS_CH4_");
  }
  if (systemParamName == "M") {
    return F("_MOTION_DETECTION_");
  }
  if (systemParamName == "C") {
    return F("_GAS_CO_");
  }
  return F("CNNT_RECOGNIZE_PARAM");
}

int16_t PARAMS_decodeParam(uint8_t paramNum, int16_t paramVal_encoded) {
  int16_t paramVal_decoded = 0;
  switch (paramNum) {
    case 0: //V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
      paramVal_decoded = paramVal_encoded - 1;
      break;
    case 1: //T   0=null, -50..120 [+100]   temperature, C
      paramVal_decoded = paramVal_encoded - 100;
      break;
    case 2: //H   0=null, 0..100   [+100]   humidity, %
      paramVal_decoded = paramVal_encoded - 100;
      break;
    case 3: //W   0=null, 100=ok 101=alert         water leak, bool
      paramVal_decoded = paramVal_encoded - 100;
      break;
    case 4: //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
      paramVal_decoded = paramVal_encoded - 1;
      break;
    case 5: //M   0=null, 100=ok 101=alert          motion detector, bool
      paramVal_decoded = paramVal_encoded - 100;
      break;
    case 6: //C   0=null, 0..1023 [+1]      gas CO, ADC value
      paramVal_decoded = paramVal_encoded - 1;
      break;
  }
  return paramVal_decoded;
}

bool PARAMS_isDangerParamValue(uint8_t paramNum, int16_t paramVal_decoded) {
  bool isDanger = false;
  switch (paramNum) {
    case 0: //V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
      isDanger = (bool)( (paramVal_decoded < 3.0) || (paramVal_decoded > 3.63) );
      break;
    case 1: //T   0=null, -50..120 [+100]   temperature, C
      isDanger = (bool)( (paramVal_decoded < 15) || (paramVal_decoded > 28) );
      break;
    case 2: //H   0=null, 0..100   [+100]   humidity, %
      isDanger = (bool)( (paramVal_decoded < 5) || (paramVal_decoded > 40) );
      break;
    case 3: //W   0=null, 100=ok 101=alert         water leak, bool
      isDanger = (bool)( paramVal_decoded == 1 );
      break;
    case 4: //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
      isDanger = (bool)( (paramVal_decoded < 555) || (paramVal_decoded > 777) );
      break;
    case 5: //M   0=null, 100=ok 101=alert          motion detector, bool
      isDanger = (bool)( paramVal_decoded == 1 );
      break;
    case 6: //C   0=null, 0..1023 [+1]      gas CO, ADC value
      isDanger = (bool)( (paramVal_decoded < 555) || (paramVal_decoded > 777) );
      break;
  }
  return isDanger;
}
