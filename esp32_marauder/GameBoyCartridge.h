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


#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif


extern Buffer buffer_obj;


class GameBoyCartridge {

  private:
    uint8_t startRomBuffer[385];
    char gameTitle[17];
    uint8_t sdBuffer[64];
    
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
    uint8_t receivedBuffer[64];
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
    uint8_t* serial_read_64_bytes(uint8_t count);
    // Read from Serial until a 0 (string terminator byte) is received
    void serial_read_chars();
    void read_and_store_chunk(uint32_t offset, uint8_t* buffer_chunk, size_t chunk_size);
    void read_and_store_chunks(uint32_t startOffset, uint8_t* buffer, size_t chunkSize, size_t numChunks);
    void read_serial_bytes(uint32_t startOffset, size_t chunkSize, uint8_t* buffer);
    String getSerialInput();

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
    void restoreRAM(uint16_t maxBufferSize, const int chunkSize);

    bool isWrittingRAM();
    bool isWrittingROM();
    bool isRestoringRAM();
    void test(uint16_t maxBufferSize);
};

#endif