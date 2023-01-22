#ifndef SD_MMCInterface_h
#define SD_MMCInterface_h

#include "configs.h"

#include "FS.h"
#include "SD_MMC.h"

#include "Buffer.h"
#ifdef HAS_SCREEN
  #include "Display.h"
#endif
#include <Update.h>

extern Buffer buffer_obj;
#ifdef HAS_SCREEN
  extern Display display_obj;
#endif

class SDInterface {

  private:
    bool checkDetectPin();

  public:
    uint8_t cardType;
    //uint64_t cardSizeBT;
    //uint64_t cardSizeKB;
    uint64_t cardSizeMB;
    //uint64_t cardSizeGB;
    bool supported = false;
    bool do_save = true;

    String card_sz;
  
    bool initSD();

    void addPacket(uint8_t* buf, uint32_t len);
    void openCapture(String file_name = "");
    void runUpdate();
    void performUpdate(Stream &updateSource, size_t updateSize);
    void main();
    //void savePacket(uint8_t* buf, uint32_t len);
};

#endif
