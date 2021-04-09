#include "a32u4_interface.h"

HardwareSerial MySerial_two(2);

void A32u4Interface::begin() {
  MySerial_two.begin(BAUD32U4, SERIAL_8N1, 25, 4);

  Serial.println("Setup A32U4 Serial Interface");

  this->initTime = millis();
}

void A32u4Interface::test() {
  MySerial_two.println("STRING Hello, World!");
}

void A32u4Interface::main(uint32_t current_time) {
/*
  if (current_time - this->initTime >= 1000) {
    this->initTime = millis();
    MySerial_two.write("PING");

    delay(1);
    
    if (MySerial_two.available()) {
      Serial.println("Got A32U4 Serial data");
      Serial.println(MySerial_two.readString());
    }
  }

  //delay(1);*/
}
