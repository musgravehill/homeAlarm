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

#define DEBUG 1

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include <SoftwareSerial.h>

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

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

SoftwareSerial sftSrl_forCommand(7, 8); // RX, TX

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 6 - Serial clock out (SCLK)
// pin 5 - Serial data out (DIN)
// pin 4 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 2 - LCD reset (RST)
Adafruit_PCD8544 myDisplay = Adafruit_PCD8544(6, 5, 4, 3, 2);

void setup() {
  delay(2000);
#ifdef DEBUG
  Serial.begin(9600);
  delay(100);
  Serial.println(F("Im Base with AckPayload"));
#endif

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
}

void loop() {
  NRF_listen();
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
#ifdef DEBUG
  Serial.print(F("Sensor# "));
  Serial.println(currPipeNum);
  Serial.print(F("size:"));
  Serial.println(sizeof(messageFromSensor), DEC);
  Serial.println(messageFromSensor[0], DEC);
  Serial.println(messageFromSensor[1], DEC);
  Serial.println(messageFromSensor[2], DEC);
  Serial.println(messageFromSensor[3], DEC);
  Serial.println(messageFromSensor[4], DEC);
  Serial.println(messageFromSensor[5], DEC);
  Serial.println(messageFromSensor[6], DEC);
  Serial.print(F("\r\n"));
  Serial.print(F("\r\n"));
#endif

  String commandToBaseSdGsmRtc_logs = "{LOGS;#" + String(currPipeNum, DEC) + ";";
  String commandToBaseSdGsmRtc_dangers = "";
  const char paramCode[] = {'V', 'T', 'H', 'W', 'G', 'M', 'C'};
  uint8_t i = 0;
  uint16_t paramVal_decoded;
  for (i = 0; i < 7; i++) {
    if (messageFromSensor[i] != 0) { //param is available
      paramVal_decoded = BASE_decodeParam(i, messageFromSensor[i]);
      commandToBaseSdGsmRtc_logs +=  String((char)paramCode[i]) + String(paramVal_decoded, DEC) + ";";
      if (BASE_isDangerParamValue(i, paramVal_decoded)) {
        commandToBaseSdGsmRtc_dangers += "{DNGR;#" + String(currPipeNum, DEC) + ";";
        commandToBaseSdGsmRtc_dangers += String((char)paramCode[i]);
        commandToBaseSdGsmRtc_dangers += String(paramVal_decoded, DEC) + ";}";
      }
    }
    else {
      commandToBaseSdGsmRtc_logs += String((char)paramCode[i]) +  "_;";
    }
  }
  commandToBaseSdGsmRtc_logs += "}";
  String commandToBaseSdGsmRtc_all = commandToBaseSdGsmRtc_logs + commandToBaseSdGsmRtc_dangers;
  sftSrl_forCommand.println(commandToBaseSdGsmRtc_all);
  
  myDisplay.clearDisplay();
  myDisplay.setTextSize(1);
  myDisplay.setTextColor(BLACK);
  myDisplay.setCursor(0, 0);
  myDisplay.println(commandToBaseSdGsmRtc_all);
  myDisplay.display();
#ifdef DEBUG
  Serial.println(commandToBaseSdGsmRtc_all);
#endif
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
