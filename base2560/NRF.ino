void NRF_listen() {
  if (radio.available(&NRF_currPipeNum)) {
    radio.writeAckPayload(NRF_currPipeNum, &NRF_currPipeNum, sizeof(NRF_currPipeNum) );
    if (radio.getDynamicPayloadSize() > 1) { //размер полученного сообщения
      radio.read(&NRF_messageFromSensor, sizeof(NRF_messageFromSensor));
      BASE_processDataFromSensor();
    }
    //else {
    // Corrupt payload has been flushed
    //}
    //radio.stopListening(); //не надо! СТОП только если хочешь write
    //radio.startListening();//не надо! СТАРТ один раз, когда объявил трубы
  }
  //bool tx, fail, rx;  //radio.whatHappened(tx, fail, rx); // What happened?  //if ( rx || radio.available()) {
}

void NRF_init() {
  radio.begin();
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

  radio.setAutoAck(true);//disallow RX send answer(acknoledgement) to TX (for ALL NRF_pipes?)
  //radio.enableAckPayload(); //custom ack //only for 0,1 NRF_pipes?
  ////radio.enableDynamicAck(); //for ALL NRF_pipes? Чтобы можно было вкл\выкл получение ACK?

  //radio.openReadingPipe(0, pipe0); 0 is SYSTEM, no reading
  radio.openReadingPipe(1, NRF_pipes[1]);
  radio.openReadingPipe(2, NRF_pipes[2]);
  radio.openReadingPipe(3, NRF_pipes[3]);
  radio.openReadingPipe(4, NRF_pipes[4]);
  radio.openReadingPipe(5, NRF_pipes[5]);
  radio.startListening();
}
