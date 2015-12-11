/*
  3.3V ardu_328
  nrf24l01+
  dht22
*/

/* Defined constants in arduino don't take up any program memory space on the chip.
  The compiler will replace references to these constants with the defined value
  at compile time.*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include "LowPower.h" //LP
#include "DHT.h"

#define IM_SENSOR_NUM 1  //1..5
#define NRF_CE_PIN 9 
#define NRF_CSN_PIN 10 //if use SPI, d10=hardware SS SPI only

const uint64_t pipes[6] = {   //'static' - no need
  0xDEADBEEF00LL,  //pipe0 is SYSTEM_pipe, avoid openReadingPipe(0, );
  0xDEADBEEF01LL,
  0xDEADBEEF02LL,
  0xDEADBEEF03LL,
  0xDEADBEEF04LL,
  0xDEADBEEF05LL
};

RF24 NRF_radio(NRF_CE_PIN, NRF_CSN_PIN);

uint16_t LP_counterSleep_8s = 0;
DHT dht;

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
  dht.setup(2); // data pin 2
  uint16_t batteryVoltage = random(0, 1023); //ADC 10 bit
  uint16_t humidity = (int) dht.getHumidity();
  uint16_t temperature = (int) dht.getTemperature();

  Serial.print(humidity);
  Serial.print("% \t t=");
  Serial.print(temperature);
  Serial.println(" ");

  NRF_sendData(batteryVoltage, temperature, humidity );
}


void NRF_init() {
  Serial.print(F("Im Sensor# "));
  Serial.print(IM_SENSOR_NUM);
  Serial.print(F("\r\n"));

  delay(50);
  NRF_radio.begin();
  delay(100);
  NRF_radio.powerUp();
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
  NRF_radio.powerDown();
  delay(50);
}

void NRF_sendData(uint16_t batteryVoltage, uint16_t temperature, uint16_t humidity) {
  uint16_t arrayToBase[3] = {
    batteryVoltage,
    temperature,
    humidity
  };

  uint8_t answerFromBase; //2^8 - 1   [0,255]

  delay(50);
  NRF_radio.powerUp();
  delay(50);

  //Stop listening for incoming messages, and switch to transmit mode.
  //Do this before calling write().
  NRF_radio.stopListening();
  NRF_radio.write( &arrayToBase, sizeof(arrayToBase));

  Serial.print(F("V= "));
  Serial.print(arrayToBase[0]);
  Serial.print(F("\r\n"));
  Serial.print(F("t= "));
  Serial.print(arrayToBase[1]);
  Serial.print(F("\r\n"));
  Serial.print(F("h= "));
  Serial.print(arrayToBase[2]);
  Serial.print(F("\r\n"));

  if ( NRF_radio.isAckPayloadAvailable() ) {
    NRF_radio.read(&answerFromBase, sizeof(answerFromBase)); //приемник принял и ответил

    Serial.print(F("__Received answer from Base: "));
    Serial.print(answerFromBase, DEC);
    Serial.print(F("\r\n"));
  }

  delay(50);
  NRF_radio.powerDown();
  delay(50);
}
