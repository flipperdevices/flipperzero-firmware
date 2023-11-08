#ifndef GameBoyCartridge_h
#define GameBoyCartridge_h

#include <iostream>
#include <string>
#include <esp32-hal-timer.h>
#include <esp32-hal-ledc.h>
#include <ArduinoJson.h>
#include "Gameboy.h"

#include "Buffer.h"
#include "GameboyServer.h"


extern Buffer buffer_obj;


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
    word romAddress = 0;
    uint8_t currentBank = 0;
    uint32_t processedProgressBar = 0;
    uint32_t totalProgressBar = 0;
    word romStartBank = 1;
    unsigned long totalRamBytesReceived = 0;
    unsigned long totalRamBytes;
    uint8_t receivedBuffer[129];
    uint16_t currentAddress = 0;


    word lastByte;
    bool runGameBoyCartridge;
    bool writtingRAM;
    bool restoringRAM;
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
    void ramEnable();
    void ramDisable();
    // Receive Serial data
    uint8_t serial_receive();
    // Transmit Serial data
    void serial_transmit(uint8_t data);
    // Read 1-128 bytes from the Serial 
    void serial_read_bytes(uint8_t count);
    // Read from Serial until a 0 (string terminator byte) is received
    void serial_read_chars();


  public:
    GameBoyCartridge();
    void main();
    void begin();
    void start();
    void stop();
    void headerROM_GB(bool printInfo);
    void readROM_GB();
    void readSRAM_GB();
    void writeByteSRAM_GB(uint16_t address, uint8_t myData);
    void writeROM_GB();
    void writeRAM_GB();
    // void startWriteRAM_GB();
    // void endWriteRAM_GB();
    // void startReadRAM_GB();
    // void endReadRAM_GB();
    void restoreRAM(size_t maxBufferSize);

    bool isWrittingRAM();
    bool isWrittingROM();
    bool isRestoringRAM();
    void test(uint16_t maxBufferSize);
};

#endif