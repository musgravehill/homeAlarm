/*
  base with NRF
  Get data from sensors 1..5 by NRF.
  Process data => make commands [logs, dngr] => send by Serial to baseSdGsmRtc
*/

/*
  NRF[encoded value in uint16_t] --> baseNRF --> decoded "command params"  --> baseSdGsmRtc

  command from baseNRF
    {LOGS;#1;V_;T21;H48;W_;G_;M_;C_;}
    {DNGR;#1;T21;}

  command params
    V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
    T   0=null, -50..120 [+100]   temperature, C
    H   0=null, 0..100   [+100]   humidity, %
    W   0=null, 0=null, (0,1 + 100) = 100=normal, 101=alert         water leak, bool
    G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    M   0=null, (0,1 + 100) = 100=normal, 101=alert motion detector, bool
    C   0=null, 0..1023 [+1]      gas CO, ADC value

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

#define NRF_CE_PIN 9
#define NRF_CSN_PIN 10 //hardware SS SPI

//'static' - no need
const uint64_t NRF_pipes[6] = {
  0xDEADBEEF00LL,  //pipe0 is SYSTEM_pipe, avoid openReadingPipe(0, );
  0xDEADBEEF01LL,
  0xDEADBEEF02LL,
  0xDEADBEEF03LL,
  0xDEADBEEF04LL,
  0xDEADBEEF05LL
};

uint8_t currPipeNum;
uint16_t messageFromSensor[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

//время последнего сигнала от сенсоров, если давно было => сенсор сломался или выключен
uint32_t millisPrevSignal_sensors[5] = {0};

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

//SoftwareSerial sftSrl_forCommand(7, 8); // RX, TX

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 6 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 4 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 2 - LCD reset (RST)
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(6, 5, 4, 3, 2);

const int LED_latchPin = A0; //ST_CP 74HC595
const int LED_clockPin = A1; //SH_CP 74HC595
const int LED_dataPin = A2; //DS 74HC595
uint16_t LED_twoBytes = 0b0;

void setup() {
  delay(2000);

  Serial.begin(9600);
  delay(100);

  radio.begin();
  delay(100);
  radio.powerUp();
  delay(100);
  radio.setChannel(0x6D);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);

  /*
    ===writeAckPayload===enableDynamicPayloads===
    !  Only three of these can be pending at any time as there are only 3 FIFO buffers.
    !  Dynamic payloads must be enabled.
    !  write an ack payload as soon as startListening() is called
  */
  radio.enableDynamicPayloads();//for ALL NRF_pipes
  //radio.setPayloadSize(32); //32 bytes? Can corrupt "writeAckPayload"?

  radio.setAutoAck(true);//allow RX send answer(acknoledgement) to TX (for ALL NRF_pipes?)
  radio.enableAckPayload(); //only for 0,1 NRF_pipes?
  //radio.enableDynamicAck(); //for ALL NRF_pipes? Чтобы можно было вкл\выкл получение ACK?

  //radio.openReadingPipe(0, pipe0); 0 is SYSTEM, no reading
  radio.openReadingPipe(1, NRF_pipes[1]);
  radio.openReadingPipe(2, NRF_pipes[2]);
  radio.openReadingPipe(3, NRF_pipes[3]);
  radio.openReadingPipe(4, NRF_pipes[4]);
  radio.openReadingPipe(5, NRF_pipes[5]);
  radio.startListening();

  delay(100);
  myDisplay.begin();
  myDisplay.setContrast(60);
  myDisplay.clearDisplay();
  myDisplay.setRotation(2);

  pinMode(LED_latchPin, OUTPUT);
  pinMode(LED_dataPin, OUTPUT);
  pinMode(LED_clockPin, OUTPUT);
  LED_twoBytes = 0b1111111111111111; //TEST LEDS
  digitalWrite(LED_latchPin, LOW); //leds off
  byte LED_twoByte1 = highByte(LED_twoBytes);
  byte LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
  delay(100);

}

void loop() {
  NRF_listen();
  BASE_checkSensorsFault();
}

void NRF_listen() {
  if (radio.available(&currPipeNum)) {
    radio.writeAckPayload(currPipeNum, &currPipeNum, sizeof(currPipeNum) );
    if (radio.getDynamicPayloadSize() > 1) { //размер полученного сообщения
      radio.read(&messageFromSensor, sizeof(messageFromSensor));
      BASE_processDataFromSensor();
    }
    else {
      // Corrupt payload has been flushed
    }
    //radio.stopListening(); //не надо! СТОП только если хочешь write
    //radio.startListening();//не надо! СТАРТ один раз, когда объявил трубы
  }
  //bool tx, fail, rx;  //radio.whatHappened(tx, fail, rx); // What happened?  //if ( rx || radio.available()) {
}

void BASE_processDataFromSensor() {

  String commandToBaseSdGsmRtc_logs = "{LOGS;#" + String(currPipeNum, DEC) + ";";
  String commandToBaseSdGsmRtc_dangers = "";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  uint8_t i = 0;
  uint16_t paramVal_decoded;

  LED_twoBytes = 0b0;
  digitalWrite(LED_latchPin, LOW); //leds off

  for (i = 0; i < 7; i++) {
    if (messageFromSensor[i] != 0) { //param is available
      paramVal_decoded = BASE_decodeParam(i, messageFromSensor[i]);
      commandToBaseSdGsmRtc_logs +=  String((char)paramCode[i]) + String(paramVal_decoded, DEC) + ";";
      if (BASE_isDangerParamValue(i, paramVal_decoded)) {
        commandToBaseSdGsmRtc_dangers += "{DNGR;#" + String(currPipeNum, DEC) + ";";
        commandToBaseSdGsmRtc_dangers += String((char)paramCode[i]);
        commandToBaseSdGsmRtc_dangers += String(paramVal_decoded, DEC) + ";}";

        bitWrite(LED_twoBytes, (i * 2), 0); //green off
        bitWrite(LED_twoBytes, (i * 2 + 1), 1); //red on
      }
      else {
        bitWrite(LED_twoBytes, (i * 2), 1); //green on
        //bitWrite(LED_twoBytes, (i * 2 + 1), 0); //red off //если был DNGR, то пусть горит постоянно до перезагрузки
      }
    }
    else {
      commandToBaseSdGsmRtc_logs += String((char)paramCode[i]) +  "_;";

      bitWrite(LED_twoBytes, (i * 2), 0); //green off
      //bitWrite(LED_twoBytes, (i * 2 + 1), 0); //red off //если был DNGR, то пусть горит постоянно до перезагрузки
    }
  }
  commandToBaseSdGsmRtc_logs += "}";
  String commandToBaseSdGsmRtc_all = commandToBaseSdGsmRtc_logs + commandToBaseSdGsmRtc_dangers;

  Serial.println(commandToBaseSdGsmRtc_all);

  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.println(commandToBaseSdGsmRtc_all);
  myDisplay.display();

  millisPrevSignal_sensors[currPipeNum] =  millis(); //save time of sensor answer

  byte LED_twoByte1 = highByte(LED_twoBytes);
  byte LED_twoByte2 = lowByte(LED_twoBytes);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte1);
  shiftOut(LED_dataPin, LED_clockPin, MSBFIRST, LED_twoByte2);
  digitalWrite(LED_latchPin, HIGH); //leds ready
}

void BASE_checkSensorsFault() {

  uint32_t millisCurrSignal = millis();
  myDisplay.fillRect(0, 40, 84, 8, 0);//clear white stripe for icons
  for (int sensorNum = 1; sensorNum <= 5; sensorNum++) { //SENSORS PIPES 1..5!
    myDisplay.fillRect(sensorNum * 12, 40, 8, 8, 1); //black icon == ok
    uint32_t deltaSignal = millisCurrSignal - millisPrevSignal_sensors[sensorNum];
    if (deltaSignal >  10000) { //10s
      //sensor fault
      myDisplay.fillRect((sensorNum * 12 + 1), 41, 6, 6, 0); //white icon == fault
    }
    else {
      //sensor ok
    }
  }
  myDisplay.display();
}

uint16_t BASE_decodeParam(uint8_t paramNum, uint16_t paramVal_encoded) {
  uint16_t paramVal_decoded = 0;
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

bool BASE_isDangerParamValue(uint8_t paramNum, uint16_t paramVal_decoded) {
  bool isDanger = false;
  switch (paramNum) {
    case 0: //V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
      isDanger = (bool)( (paramVal_decoded < 555) || (paramVal_decoded > 777) );
      break;
    case 1: //T   0=null, -50..120 [+100]   temperature, C
      isDanger = (bool)( (paramVal_decoded < 15) || (paramVal_decoded > 19) );
      break;
    case 2: //H   0=null, 0..100   [+100]   humidity, %
      isDanger = (bool)( (paramVal_decoded < 5) || (paramVal_decoded > 40) );
      break;
    case 3: //W   0=null, 100=ok 101=alert         water leak, bool
      isDanger = (bool)( paramVal_decoded == 101 );
      break;
    case 4: //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
      isDanger = (bool)( (paramVal_decoded < 555) || (paramVal_decoded > 777) );
      break;
    case 5: //M   0=null, 100=ok 101=alert          motion detector, bool
      isDanger = (bool)( paramVal_decoded == 101 );
      break;
    case 6: //C   0=null, 0..1023 [+1]      gas CO, ADC value
      isDanger = (bool)( (paramVal_decoded < 555) || (paramVal_decoded > 777) );
      break;
  }
  return isDanger;
}

void LED_paramsState() {
}
