#ifndef a32u4_interface_h
#define a32u4_interface_h

#include "Display.h"
#include <HardwareSerial.h>

#define BAUD32U4 115200

extern Display display_obj;

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
