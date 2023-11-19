#include "eepromclass.h"
#include <Arduino.h>

EEPROMclass::EEPROMclass()
{
}

void EEPROMclass::RunSetup(){
  EEPROM.begin(eeprom_size);
}

void EEPROMclass::writeEEPROMint(int index, int value){
  EEPROM.write(index, value);
  EEPROM.commit();
}

int EEPROMclass::readEEPROMint(int index){
    int value = EEPROM.read(index);
    return value;
}

int EEPROMclass::writeStringToEEPROM(int addrOffset, const String &strToWrite){
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int EEPROMclass::readStringFromEEPROM(int addrOffset, String *strToRead){
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // the character may appear in a weird way, you should read: 'only one backslash and 0'
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

void EEPROMclass::clearEEPROM(){
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
    Serial.println(i);
    if(i == EEPROM.length()-1){
      EEPROM.commit();
      ESP.reset();
    }
  }
}
