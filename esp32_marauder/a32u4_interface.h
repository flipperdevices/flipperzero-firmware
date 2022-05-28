#ifndef a32u4_interface_h
#define a32u4_interface_h

#include "configs.h"

#ifdef HAS_SCREEN
  #include "Display.h"
#endif
#include <HardwareSerial.h>

#define BAUD32U4 115200

#ifdef HAS_SCREEN
  extern Display display_obj;
#endif

class A32u4Interface {
  public:
    bool supported = false;

    uint32_t initTime;

    void begin();

    void main(uint32_t current_time);
    void test();
    void runScript(String script);
};

#endif
