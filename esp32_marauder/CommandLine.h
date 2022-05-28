#ifndef CommandLine_h
#define CommandLine_h

#include "configs.h"

#ifdef HAS_SCREEN
  #include "MenuFunctions.h"
  #include "Display.h"
#endif 

#include "WiFiScan.h"

#ifdef HAS_SCREEN
  extern MenuFunctions menu_function_obj;
  extern Display display_obj;
#endif

extern WiFiScan wifi_scan_obj;

// Commands
const char PROGMEM CH_CMD[] = "channel";
const char PROGMEM SCANAP_CMD[] = "scanap";
const char PROGMEM SNIFF_BEACON_CMD[] = "sniffbeacon";
const char PROGMEM SNIFF_DEAUTH_CMD[] = "sniffdeauth";
const char PROGMEM SNIFF_PMKID_CMD[] = "sniffpmkid";
const char PROGMEM STOPSCAN_CMD[] = "stopscan";
const char PROGMEM CLEARAP_CMD[] = "clearap";

class CommandLine {
  private:
    String getSerialInput();
    LinkedList<String> parseCommand(String input);
    void runCommand(String input);
    int argSearch(LinkedList<String>* cmd_args, String key);
        
  public:
    CommandLine();

    void RunSetup();
    void main(uint32_t currentTime);
};

#endif
