#ifndef GameBoyCartridge_h
#define GameBoyCartridge_h

#include <iostream>
#include <string>
#include <esp32-hal-timer.h>
#include <esp32-hal-ledc.h>
#include <ArduinoJson.h>
#include "Gameboy.h"

#include "Buffer.h"

extern Buffer buffer_obj; 

DynamicJsonDocument transferJSON(2048);

class GameBoyCartridge {

  private:
    uint8_t startRomBuffer[385];
    char gameTitle[17];
    
    word lastByte;
    bool runGameBoyCartridge;
    void setup();
    
    byte read_byte_GB(uint16_t address);
    void write_byte_GB(int address, byte data);
    void set_address_GB(uint16_t address);
    void gb_mode(void);
   
    byte readByteSRAM_GB(uint16_t myAddress);
    void rd_wr_mreq_off(void);
    void rd_wr_mreq_reset(void);
    void rd_wr_csmreq_cs2_reset(void);
    void dataBusAsOutput();
    void dataBusAsInput();

  public:
    GameBoyCartridge();
    void main();
    void begin();
    void start();
    void stop();
    void headerROM_GB(bool printInfo);
    void readROM_GB();
    void readSRAM_GB();

};

#endif