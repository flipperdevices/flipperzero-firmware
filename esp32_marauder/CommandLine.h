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
extern LinkedList<AccessPoint>* access_points;
extern const String PROGMEM version_number;

//// Commands

// Admin
const char PROGMEM CH_CMD[] = "channel";
const char PROGMEM CLEARAP_CMD[] = "clearap";
const char PROGMEM REBOOT_CMD[] = "reboot";

// WiFi sniff/scan
const char PROGMEM SCANAP_CMD[] = "scanap";
const char PROGMEM SNIFF_BEACON_CMD[] = "sniffbeacon";
const char PROGMEM SNIFF_DEAUTH_CMD[] = "sniffdeauth";
const char PROGMEM SNIFF_PMKID_CMD[] = "sniffpmkid";
const char PROGMEM STOPSCAN_CMD[] = "stopscan";

// WiFi attack
const char PROGMEM ATTACK_CMD[] = "attack";
const char PROGMEM ATTACK_TYPE_DEAUTH[] = "deauth";
const char PROGMEM ATTACK_TYPE_BEACON[] = "beacon";
const char PROGMEM ATTACK_TYPE_PROBE[] = "probe";

// WiFi Aux
const char PROGMEM LIST_AP_CMD[] = "listap";
const char PROGMEM SEL_CMD[] = "select";

class CommandLine {
  private:
    String getSerialInput();
    LinkedList<String> parseCommand(String input, char* delim);
    void runCommand(String input);
    int argSearch(LinkedList<String>* cmd_args, String key);

    const char* ascii_art =
    "\r\n"
    "              @@@@@@                        \r\n"
    "              @@@@@@@@                      \r\n"
    "              @@@@@@@@@@@                   \r\n"
    "             @@@@@@  @@@@@@                 \r\n"
    "          @@@@@@@      @@@@@@@              \r\n"
    "        @@@@@@            @@@@@@            \r\n"
    "     @@@@@@@                @@@@@@@         \r\n"
    "   @@@@@@                      @@@@@@       \r\n"
    "@@@@@@@              @@@@@@@@@@@@@@@@       \r\n"
    "@@@@@                 @@@@@@@@@@@@@@@       \r\n"
    "@@@@@                   @@@@@@@             \r\n"
    "@@@@@                      @@@@@@           \r\n"
    "@@@@@@                       @@@@@@@        \r\n"
    "  @@@@@@                        @@@@@@@@@@@@\r\n"
    "    @@@@@@@                          @@@@@@ \r\n"
    "       @@@@@@                     @@@@@@    \r\n"
    "         @@@@@@@                @@@@@@      \r\n"
    "            @@@@@@           @@@@@@         \r\n"
    "              @@@@@@@      @@@@@@           \r\n"
    "                 @@@@@@ @@@@@@              \r\n"
    "                   @@@@@@@@@                \r\n"
    "                      @@@@@@                \r\n"
    "                        @@@@                \r\n"
    "\r\n";
        
  public:
    CommandLine();

    void RunSetup();
    void main(uint32_t currentTime);
};

#endif
