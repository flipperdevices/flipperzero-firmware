#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "IFTTTMode.h"
#include "Global.h"
#include "eepromclass.h"

IFTTTMode iftttmode_obj;
EEPROMclass eeprom_obj;

#define FLIPPERZERO_SERIAL_BAUD 115200

String incomingString;
String tmpString;
String FirstKeyCommand;

void CheckForFlipperCommands()
{
  while(Serial.available() > 0)
  {    
    incomingString = "";
    tmpString = "";
    while (Serial.available()) {
      delay(2);
      int c = Serial.read();
      tmpString = (char) c;
      incomingString += tmpString;
    }
    FirstKeyCommand = getValue(incomingString, ',', 0);
    if(FirstKeyCommand == "config"){
      iftttmode_obj.keyIFTTT = getValue(incomingString, ',', 1);
      String ssid = getValue(incomingString, ',', 2);
      String password = getValue(incomingString, ',', 3);
      iftttmode_obj.eventIFTTT = getValue(incomingString, ',', 4);
      iftttmode_obj.ssidIFTTT = ssid.c_str();
      iftttmode_obj.passwordIFTTT = password.c_str();
      iftttmode_obj.RunSetup();
      FirstKeyCommand = "";
    }
    if(incomingString == "send")
    {
      incomingString = "";
      iftttmode_obj.IFTTTCommand();
    }
    if(incomingString == "reboot")
    {
      incomingString = "";
      ESP.restart();
    }
    if(incomingString == "reset")
    {
      incomingString = "";
      eeprom_obj.clearEEPROM();
    }
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(FLIPPERZERO_SERIAL_BAUD);
  eeprom_obj.RunSetup();
  incomingString = "";
  tmpString = "";
}

void loop() {   
  CheckForFlipperCommands();
}
