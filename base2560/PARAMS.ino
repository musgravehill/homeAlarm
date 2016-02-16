String PARAMS_getVerbalParamName(uint8_t paramNum) {
  if (paramNum == 0) {
    return F("Batt");
  }
  if (paramNum == 1) {
    return F("Temp-ra");
  }
  if (paramNum == 2) {
    return F("Vlaga");
  }
  if (paramNum == 3) {
    return F("Proteklo");
  }
  if (paramNum == 4) {
    return F("Gaz");
  }
  if (paramNum == 5) {
    return F("Dvigaet");
  }
  if (paramNum == 6) {
    return F("Ugar");
  }
  return F("?PARAM");
}

int16_t PARAMS_decodeParam(uint8_t paramNum, int16_t paramVal_encoded) {
  int16_t paramVal_decoded = 0;
  switch (paramNum) {
    case 0: //0=null, 100*V.**  voltage on sensor battery, 100*V
      paramVal_decoded = paramVal_encoded; //100*V , cannot use float
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
    case 0: //0=null, 100*V,**  voltage on sensor battery, 100*V
      isDanger = (bool) (paramVal_decoded < 310) ; //100*V
      break;
    case 1: //T   0=null, -50..120 [+100]   temperature, C
      isDanger = (bool)( (paramVal_decoded < 15) || (paramVal_decoded > 20) );
      break;
    case 2: //H   0=null, 0..100   [+100]   humidity, %
      isDanger = (bool)( (paramVal_decoded < 5) || (paramVal_decoded > 60) );
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
