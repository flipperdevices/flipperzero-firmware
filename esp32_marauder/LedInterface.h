#ifndef LedInterface_h
#define LedInterface_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 25
#define Pixels 1

extern Adafruit_NeoPixel strip;

class LedInterface {

  private:
    uint32_t initTime = 0;

    int current_fade_itter = 1;
    int wheel_pos = 255;
    int wheel_speed = 1; // lower = slower

    uint32_t Wheel(byte WheelPos);
  
  public:
    LedInterface();

    void RunSetup();
    void main(uint32_t currentTime);
  
};

#endif
