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
    
    uint16_t cartridgeType;
    uint16_t romSize;
    uint16_t romBanks;
    uint16_t ramSize;
    uint16_t ramBanks;
    uint16_t ramEndAddress;
    unsigned long sramSize = 0;
    word romEndAddress;
    word sramBanks;
    int romType;


    word lastByte;
    bool runGameBoyCartridge;
    bool writtingRAM;
    bool writtingROM;
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
    void writeROM_GB();
    void writeRAM_GB();
    void startWriteRAM_GB();
    void endWriteRAM_GB();

    bool isWrittingRAM();
    bool isWrittingROM();
};

#endif