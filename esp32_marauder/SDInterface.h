#ifndef SDInterface_h
#define SDInterface_h

#include "SD.h"

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
  
    bool initSD();
  
};

#endif
