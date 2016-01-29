/*
  ardu_328
  nrf24l01+
  dht22
  PIR
*/
/* Defined constants in arduino don't take up any program memory space on the chip.
  The compiler will replace references to these constants with the defined value
  at compile time.
*/
/*
  command params
    //100*V.xx 0=null, voltage on sensor battery, 100*V
    //T   0=null, -50..120 [+100]   temperature, C
    //H   0=null, 0..100   [+100]   humidity, %
    //W   0=null, 100, 101          water leak, bool
    //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    //M   0=null, 100, 101          motion detector, bool
    //C   0=null, 0..1023 [+1]      gas CO, ADC value

  LOGS => log on SD only
  DNGR => log on SD & send SMS [danger]
*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>
#include "LowPower.h" //LP
#include "DHT.h"
#include <avr/wdt.h>

#define IM_SENSOR_NUM 1  //1..5
#define NRF_CE_PIN 9
#define NRF_CSN_PIN 10 //if use SPI, d10=hardware SS SPI only
#define BATT_CONTROL_PIN_1V1 A0 //hardcoded in PCB voltage divider
#define DHT22_DATA_PIN 2
#define PIR_DATA_PIN 3

const uint64_t pipes[6] = {   //'static' - no need
  0xDEADBEEF00LL,  //pipe0 is SYSTEM_pipe, avoid openReadingPipe(0, );
  0xDEADBEEF01LL,
  0xDEADBEEF02LL,
  0xDEADBEEF03LL,
  0xDEADBEEF04LL,
  0xDEADBEEF05LL
};

RF24 NRF_radio(NRF_CE_PIN, NRF_CSN_PIN);

DHT dht;

uint8_t PIR_motionDetected = 0;

void setup() {
  MCUSR = 0;  //VERY VERY IMPORTANT!!!! ELSE WDT DOESNOT RESET, DOESNOT DISABLED!!!
  wdt_disable();

  pinMode(PIR_DATA_PIN, INPUT);
  delay(61000); //for PIR calibrating 60s, reduction of power fluctuation on powerUp, etc
  NRF_init();
  dht.setup(DHT22_DATA_PIN);

  //use the 1.1 V internal reference => other A* can NOT receive VCC, only 1.1V max
  /*
    DEFAULT: опорное напряжение по умолчанию, равное 5 В (на 5В-платах Ардуино) или 3.3 В (на 3.3В-платах Ардуино)
    INTERNAL: внутренне опорное напряжение, равное 1.1 В в микроконтроллерах ATmega168 и ATmega328, или 2.56 В в микроконтроллере ATmega8 (не доступно в Arduino Mega)
    INTERNAL1V1: внутреннее опорное напряжение 1.1 В (только для Arduino Mega)
    INTERNAL2V56: внутреннее опорное напряжение 2.56 В (только для Arduino Mega)
    EXTERNAL: в качестве опорного напряжения будет использоваться напряжение, приложенное к выводу AREF (от 0 до 5В)
  */
  analogReference(INTERNAL);
}

void loop() {
  wdt_enable (WDTO_8S);
  //try to have time < 8s, else autoreset by watchdog
  sendDataToBase();
  wdt_reset();
  wdt_disable();

  //Serial.flush(); //the system is going to sleep while it's still sending the serial data.
  uint8_t countSleep = 0;
  while (countSleep < 8) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    PIR_motionDetected = PIR_motionDetected | digitalRead(PIR_DATA_PIN); //Bitwise OR
    countSleep++;
  }

  //reset base after 1 day uptime
  if ((int) millis() > 86400000L) {
    wdt_enable(WDTO_2S);
    delay(2500);
  }
}

void sendDataToBase() {
  dht.setup(DHT22_DATA_PIN);
  delay(100);
  uint16_t humidity = (int) dht.getHumidity();
  delay(20);
  uint16_t temperature = (int) dht.getTemperature();
  delay(20);
  uint16_t batteryVoltage = 0.3334 * analogRead(BATT_CONTROL_PIN_1V1); // 100 * 3.24V = 324
  PIR_motionDetected = PIR_motionDetected | digitalRead(PIR_DATA_PIN); //Bitwise OR

  // 1M, 470K divider across battery and using internal ADC ref of 1.1V
  // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  // 3.44/1023 = Volts per bit = 0.003363075

  int16_t arrayToBase[7] = {
    batteryVoltage,               //100*V.xx 0=null, voltage on sensor battery, 100*V
    temperature + 100,            //T   0=null, -50..120 [+100]   temperature, C
    humidity + 100,               //H   0=null, 0..100   [+100]   humidity, %
    0,                            //W   0=null, 100, 101          water leak, bool
    0,                            //G   0=null, 0..1023 [+1] ADC  gas CH4, ADC value
    PIR_motionDetected + 100 ,  //M   0=null, 100, 101          motion detector, bool
    0,                            //C   0=null, 0..1023 [+1]      gas CO, ADC value
  };

  //Serial.print(humidity);
  //Serial.print("% \t t=");
  //Serial.print(temperature);
  //Serial.println(" ");
  if ((temperature != 0) || (humidity != 0)) {
    NRF_sendData(arrayToBase, sizeof(arrayToBase));
  }

}


void NRF_init() {
  //Serial.print(F("Im Sensor# "));
  //Serial.print(IM_SENSOR_NUM);
  //Serial.print(F("\r\n"));

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

  NRF_radio.setAutoAck(true);////allow RX send answer(acknoledgement) to TX (for ALL pipes?)
  //NRF_radio.enableAckPayload(); //custom ack //only for 0,1 pipes?
  ////NRF_radio.enableDynamicAck(); //for ALL pipes? Чтобы можно было вкл\выкл получение ACK?

  NRF_radio.stopListening();// ?
  NRF_radio.openWritingPipe(pipes[IM_SENSOR_NUM]); //pipe0 is SYSTEM_pipe, no reading

  delay(50);
  NRF_radio.powerDown();
  delay(50);
}

void NRF_sendData(int16_t* arrayToBase, uint8_t sizeofArrayToBase) {


  //Serial.println("\r\n");
  //Serial.print("arr[");
  //Serial.print(sizeofArrayToBase, DEC);
  //Serial.println("]: ");
  //Serial.println(arrayToBase[0], DEC);
  //Serial.println(arrayToBase[1], DEC);
  //Serial.println(arrayToBase[2], DEC);
  //Serial.println(arrayToBase[3], DEC);
  //Serial.println(arrayToBase[4], DEC);
  //Serial.println(arrayToBase[5], DEC);
  //Serial.println(arrayToBase[6], DEC);
  //Serial.println("\r\n");

  delay(50);
  NRF_radio.powerUp();
  delay(50);

  //Stop listening for incoming messages, and switch to transmit mode.
  //Do this before calling write().
  NRF_radio.stopListening();
  NRF_radio.write( arrayToBase, sizeofArrayToBase);
  //& не надо, в ф-ю уже передал указатель, а не сам массив

  /*
    uint8_t answerFromBase; //2^8 - 1   [0,255]
    if ( NRF_radio.isAckPayloadAvailable() ) {
      NRF_radio.read(&answerFromBase, sizeof(answerFromBase)); //приемник принял и ответил

      //Serial.print(F("__Received answer from Base: "));
      //Serial.print(answerFromBase, DEC);
      //Serial.print(F("\r\n"));
    }
  */

  delay(50);
  NRF_radio.powerDown();
  delay(50);
}
