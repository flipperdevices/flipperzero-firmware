#ifndef SDInterface_h
#define SDInterface_h

#include "SD.h"
#include "Buffer.h"

extern Buffer buffer_obj;

#define SD_CS 12

class SDInterface {

  private:

  public:
    uint8_t cardType;
    uint64_t cardSizeBT;
    uint64_t cardSizeKB;
    uint64_t cardSizeMB;
    uint64_t cardSizeGB;
    bool supported = false;
    bool do_save = true;

    String card_sz;
  
    bool initSD();

    void addPacket(uint8_t* buf, uint32_t len);
    void main();
    //void savePacket(uint8_t* buf, uint32_t len);
};

#endif
