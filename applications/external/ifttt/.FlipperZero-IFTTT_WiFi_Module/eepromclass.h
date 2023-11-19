#ifndef EEPROMclass_h
#define EEPROMclass_h
#include <Arduino.h>

#include <EEPROM.h>

class EEPROMclass
{
  private:
    int eeprom_size = 1025;
    bool isempty = false;
  public:
    EEPROMclass();
    void RunSetup();
    void clearEEPROM();
    int readEEPROMint(int index);
    void writeEEPROMint(int index, int value);
    int writeStringToEEPROM(int addrOffset, const String &strToWrite);
    int readStringFromEEPROM(int addrOffset, String *strToRead);
};
#endif
