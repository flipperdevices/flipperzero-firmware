#ifndef IFTTTMode_h
#define IFTTTMode_h

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

class IFTTTMode
{
  private:
  public:
    const char *ssidIFTTT;
    const char *passwordIFTTT;
    String keyIFTTT;
    String eventIFTTT;
    IFTTTMode();
    void RunSetup();
    void IFTTTCommand();
};
#endif
