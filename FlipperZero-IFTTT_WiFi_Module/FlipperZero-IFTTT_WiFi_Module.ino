#include <Arduino.h>

#include "APMode.h"
#include "IFTTTMode.h"
#include "Global.h"
#include "eepromclass.h"

APMode apmode_obj;
IFTTTMode iftttmode_obj;
EEPROMclass eeprom_obj;

#define MODULE_CONTROL_COMMAND_SEND 's'
#define MODULE_CONTROL_COMMAND_REBOOT 'r'
#define MODULE_CONTROL_COMMAND_CONFIG_ON 'c'
#define MODULE_CONTROL_COMMAND_CONFIG_OFF 'd'
#define MODULE_CONTROL_COMMAND_RESET 'a'
#define FLIPPERZERO_SERIAL_BAUD 115200

int runMode = 0;

void CheckForFlipperCommands()
{
  while(Serial.available() > 0)
  {    
    int incommingCommand = Serial.read();
    if(incommingCommand == MODULE_CONTROL_COMMAND_SEND)
    {
      iftttmode_obj.IFTTTCommand();
    }
    if(incommingCommand == MODULE_CONTROL_COMMAND_REBOOT)
    {
      ESP.restart();
    }
    if(incommingCommand == MODULE_CONTROL_COMMAND_CONFIG_ON)
    {
      eeprom_obj.writeEEPROMint(0,1);
      delay(10);
      ESP.restart();
    }
    if(incommingCommand == MODULE_CONTROL_COMMAND_CONFIG_OFF)
    {
      eeprom_obj.writeEEPROMint(0,0);
      delay(10);
      ESP.restart();
    }
    if(incommingCommand == MODULE_CONTROL_COMMAND_RESET)
    {
      eeprom_obj.clearEEPROM();
    }
  }
}

void setup() {
  Serial.begin(FLIPPERZERO_SERIAL_BAUD);
  eeprom_obj.RunSetup();
  runMode =  eeprom_obj.readEEPROMint(0);
  delay(10);
  if(runMode == 0){
    iftttmode_obj.RunSetup();
  }else if(runMode == 1){
    apmode_obj.RunSetup();
  }
}

void loop() {
  CheckForFlipperCommands();
  if(runMode == 1){
    apmode_obj.APCommand();
  }
}
