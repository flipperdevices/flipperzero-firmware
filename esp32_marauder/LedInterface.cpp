#include "LedInterface.h"

LedInterface::LedInterface() {
  
}

void LedInterface::RunSetup() {
  Serial.println("Setting neopixel to black...");
  strip.setBrightness(0);
  strip.begin();
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  delay(100);
  strip.setBrightness(50);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  this->initTime = millis();
}

void LedInterface::main(uint32_t currentTime) {
  strip.setPixelColor(0, this->Wheel((0 * 256 / 100 + this->wheel_pos) % 256));
  strip.show();
    
  this->current_fade_itter++;

  this->wheel_pos = this->wheel_pos - this->wheel_speed;
  if (this->wheel_pos < 0)
    this->wheel_pos = 255;
};

uint32_t LedInterface::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
