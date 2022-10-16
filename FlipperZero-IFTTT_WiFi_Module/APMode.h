#ifndef APMode_h
#define APMode_h

#include <Arduino.h>

#include <ESP8266WiFi.h>

class APMode
{
  private:
    void APindex();
  public:
    APMode();
    void RunSetup();
    void APCommand();
};
#endif
