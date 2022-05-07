#ifndef CommandLine_h
#define CommandLine_h

#include "MenuFunctions.h"
#include "WiFiScan.h"
#include "Display.h"

extern MenuFunctions menu_function_obj;
extern WiFiScan wifi_scan_obj;
extern Display display_obj;

// Commands
const char PROGMEM SCANAP_CMD[] = "scanap";
const char PROGMEM STOPSCAN_CMD[] = "stopscan";
const char PROGMEM CLEARAP_CMD[] = "clearap";

class CommandLine {
  private:
    String getSerialInput();
    void parseCommand(String input);
        
  public:
    CommandLine();

    void RunSetup();
    void main(uint32_t currentTime);
};

#endif
