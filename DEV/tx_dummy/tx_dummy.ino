/*
  ardu_328
  nrf24l01+  
*/
/* Defined constants in arduino don't take up any program memory space on the chip.
  The compiler will replace references to these constants with the defined value
  at compile time.
*/
/*
  command params
    V   n, 0..5         voltage on sensor battery, V
    T   n, -50..120     temperature, C
    H   n, 0..100       humidity, %
    W   n, 0, 1         water leak, bool
    G   n, 0..1023 ADC  gas CH4, ADC value
    M   n, 0, 1         motion detector, bool
    C   n, 0..1023      gas CO, ADC value

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include "LowPower.h" //LP

#define IM_SENSOR_NUM 3  //1..5
#define NRF_CE_PIN 9
#define NRF_CSN_PIN 10 //if use SPI, d10=hardware SS SPI only

const uint64_t pipes[6] = {   //'static' - no need
  0xDEADBEEF00LL,  //pipe0 is SYSTEM_pipe, avoid openReadingPipe(0, );
  0xDEADBEEF01LL,
  0xDEADBEEF02LL,
  0xDEADBEEF03LL,
  0xDEADBEEF04LL,
  0xDEADBEEF05LL,
};

RF24 NRF_radio(NRF_CE_PIN, NRF_CSN_PIN);

uint16_t LP_counterSleep_8s = 0;

void setup() {
  delay(2000);
  Serial.begin(9600);
  NRF_init();
}

void loop() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LP_counterSleep_8s++;

  if (LP_counterSleep_8s > 0) {
    LP_counterSleep_8s = 0;
    sendDataToBase();
  }
}

void sendDataToBase() {
  int16_t arrayToBase[7] = {
    0,                  //V   0=null, 0..1023 [+1] ADC  voltage on sensor battery, V
    126,  //T   0=null, -50..120 [+100]   temperature, C
    148,     //H   0=null, 0..100   [+100]   humidity, %
    100,                  //W   0=null, 100, 101          water leak, bool
    0,                  //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    101,                  //M   0=null, 100, 101         motion detector, bool
    0,                  //C   0=null, 0..1023 [+1]      gas CO, ADC value
  };  

  NRF_sendData(arrayToBase, sizeof(arrayToBase));
}


void NRF_init() {

  delay(50);
  NRF_radio.begin();
  delay(100);
  //NRF_radio.powerUp();
  delay(50);
  NRF_radio.setChannel(0x6D);
  NRF_radio.setRetries(15, 15);
  NRF_radio.setDataRate(RF24_1MBPS);
  NRF_radio.setPALevel(RF24_PA_LOW);
  NRF_radio.setCRCLength(RF24_CRC_16);

  /*
    ===writeAckPayload===enableDynamicPayloads===
    !  Only three of these can be pending at any time as there are only 3 FIFO buffers.
    !  Dynamic payloads must be enabled.
    !  write an ack payload as soon as startListening() is called
  */
  NRF_radio.enableDynamicPayloads();//for ALL pipes
  //NRF_radio.setPayloadSize(32); //32 bytes? Can corrupt "writeAckPayload"?

  NRF_radio.setAutoAck(true);//allow RX send answer(acknoledgement) to TX (for ALL pipes?)
  NRF_radio.enableAckPayload(); //only for 0,1 pipes?
  //NRF_radio.enableDynamicAck(); //for ALL pipes? Чтобы можно было вкл\выкл получение ACK?

  NRF_radio.stopListening();// ?
  NRF_radio.openWritingPipe(pipes[IM_SENSOR_NUM]); //pipe0 is SYSTEM_pipe, no reading

  delay(50);
  //NRF_radio.powerDown();
  delay(50);
}

void NRF_sendData(int16_t* arrayToBase, uint8_t sizeofArrayToBase) {
  uint8_t answerFromBase; //2^8 - 1   [0,255]

  Serial.println("\r\n");
  Serial.print("arr[");
  Serial.print(sizeofArrayToBase, DEC);
  Serial.println("]: ");
  Serial.println(arrayToBase[0], DEC);
  Serial.println(arrayToBase[1], DEC);
  Serial.println(arrayToBase[2], DEC);
  Serial.println(arrayToBase[3], DEC);
  Serial.println(arrayToBase[4], DEC);
  Serial.println(arrayToBase[5], DEC);
  Serial.println(arrayToBase[6], DEC);
  Serial.println("\r\n");

  delay(50);
  //NRF_radio.powerUp();
  delay(50);

  //Stop listening for incoming messages, and switch to transmit mode.
  //Do this before calling write().
  NRF_radio.stopListening();
  NRF_radio.write( arrayToBase, sizeofArrayToBase); 
  //& не надо, в ф-ю уже передал указатель, а не сам массив

  if ( NRF_radio.isAckPayloadAvailable() ) {
    NRF_radio.read(&answerFromBase, sizeof(answerFromBase)); //приемник принял и ответил

    Serial.print(F("__Received answer from Base: "));
    Serial.print(answerFromBase, DEC);
    Serial.print(F("\r\n"));
  }

  delay(50);
  //NRF_radio.powerDown();
  delay(50);
}
