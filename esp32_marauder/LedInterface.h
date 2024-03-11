#pragma once

#ifndef LedInterface_h
#define LedInterface_h

#include "configs.h"
#include "settings.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define Pixels 1

#define MODE_OFF 0
#define MODE_RAINBOW 1
#define MODE_ATTACK 2
#define MODE_SNIFF 3
#define MODE_CUSTOM 4

extern Settings settings_obj;
extern Adafruit_NeoPixel strip;

class LedInterface {

  private:
    uint32_t initTime = 0;

    int current_fade_itter = 1;
    int wheel_pos = 255;
    int wheel_speed = 1; // lower = slower

    uint32_t Wheel(byte WheelPos);

    uint8_t current_mode = MODE_OFF;

    void rainbow();
    void ledOff();
    void attackLed();
    void sniffLed();
  
  public:
    LedInterface();

    void RunSetup();
    void main(uint32_t currentTime);

    void setMode(uint8_t);
    void setColor(int r, int g, int b);
    uint8_t getMode();
    
  
};

#endif
