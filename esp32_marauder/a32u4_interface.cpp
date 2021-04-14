#include "a32u4_interface.h"

HardwareSerial MySerial_two(2);

void A32u4Interface::begin() {
  MySerial_two.begin(BAUD32U4, SERIAL_8N1, 25, 4);

  delay(2000);

  Serial.println("Setup A32U4 Serial Interface");

  MySerial_two.println("DELAY 1");

  delay(1000);

  uint8_t a32u4_rep = 0;

  if (MySerial_two.available()) {
    a32u4_rep = (uint8_t)MySerial_two.read();
  }

  //display_string.trim();

  //Serial.println("\nDisplay string: " + (String)display_string);

  if (a32u4_rep != 0) {
    this->supported = true;
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.tft.println("ATmega32U4 Found!");
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
  }
  else {
    display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
    display_obj.tft.println("ATmega32U4 Not Found");
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    Serial.print("A32U4 Said: ");
    Serial.println(a32u4_rep);
  }

  this->initTime = millis();
}

void A32u4Interface::runScript(String script) {
  MySerial_two.println(script);
}

void A32u4Interface::test() {
  MySerial_two.println("STRING Hello, World!");
}

void A32u4Interface::main(uint32_t current_time) {

  if (current_time - this->initTime >= 1000) {
    this->initTime = millis();
    //MySerial_two.write("PING");

    //delay(1);
    
    if (MySerial_two.available()) {
      Serial.println("Got A32U4 Serial data");
      Serial.println(MySerial_two.read());
    }
  }

  //delay(1);*/
}
