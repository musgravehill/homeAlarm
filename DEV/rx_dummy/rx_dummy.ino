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
int16_t messageFromSensor[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};


RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);



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

  radio.setAutoAck(false);//allow RX send answer(acknoledgement) to TX (for ALL NRF_pipes?)
  //radio.enableAckPayload(); //only for 0,1 NRF_pipes?
  //radio.enableDynamicAck(); //for ALL NRF_pipes? Чтобы можно было вкл\выкл получение ACK?

  //radio.openReadingPipe(0, pipe0); 0 is SYSTEM, no reading
  radio.openReadingPipe(1, NRF_pipes[1]);
  radio.openReadingPipe(2, NRF_pipes[2]);
  radio.openReadingPipe(3, NRF_pipes[3]);
  radio.openReadingPipe(4, NRF_pipes[4]);
  radio.openReadingPipe(5, NRF_pipes[5]);
  radio.startListening();

}

void loop() {
  NRF_listen();

}

void NRF_listen() {
  if (radio.available(&currPipeNum)) {
    //radio.writeAckPayload(currPipeNum, &currPipeNum, sizeof(currPipeNum) );
    if (radio.getDynamicPayloadSize() > 1) { //размер полученного сообщения
      radio.read(&messageFromSensor, sizeof(messageFromSensor));
      BASE_processDataFromSensor();
    }    
    //radio.stopListening(); //не надо! СТОП только если хочешь write
    //radio.startListening();//не надо! СТАРТ один раз, когда объявил трубы
  }
  //bool tx, fail, rx;  //radio.whatHappened(tx, fail, rx); // What happened?  //if ( rx || radio.available()) {
}

void BASE_processDataFromSensor() {
  for (uint8_t i = 0; i < 7; i++) {
    Serial.println(messageFromSensor[i]);
  }
  Serial.println(" ");
}

