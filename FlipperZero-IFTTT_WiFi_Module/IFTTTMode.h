#ifndef IFTTTMode_h
#define IFTTTMode_h

#include <Arduino.h>

#include <ESP8266WiFi.h>

class IFTTTMode
{
  private:
  public:
    IFTTTMode();
    void RunSetup();
    void IFTTTCommand();
};
#endif
