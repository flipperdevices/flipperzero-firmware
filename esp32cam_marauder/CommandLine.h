#pragma once

#ifndef CommandLine_h
#define CommandLine_h

#include "configs.h"

#ifdef HAS_SCREEN
  #include "MenuFunctions.h"
  #include "Display.h"
#endif

#include "WiFiScan.h"
//#include "Web.h"
#ifdef HAS_SD
  #include "SDInterface.h"
#endif
#include "settings.h"
#include "LedInterface.h"

#ifdef HAS_SCREEN
  extern MenuFunctions menu_function_obj;
  extern Display display_obj;
#endif

extern WiFiScan wifi_scan_obj;
//extern Web web_obj;
#ifdef HAS_SD
  extern SDInterface sd_obj;
#endif
extern Settings settings_obj;
extern LedInterface led_obj;
extern LinkedList<AccessPoint>* access_points;
extern LinkedList<ssid>* ssids;
extern LinkedList<Station>* stations;
extern const String PROGMEM version_number;
extern const String PROGMEM board_target;

//// Commands
// Camera functions
const char PROGMEM CAM_PHOTO[] = "photo";
const char PROGMEM CAM_FLASHLIGHT[] = "flashlight";

// Admin
const char PROGMEM CH_CMD[] = "channel";
const char PROGMEM CLEARAP_CMD[] = "clearlist";
const char PROGMEM REBOOT_CMD[] = "reboot";
const char PROGMEM UPDATE_CMD[] = "update";
const char PROGMEM HELP_CMD[] = "help";
const char PROGMEM SETTINGS_CMD[] = "settings";
const char PROGMEM LS_CMD[] = "ls";
const char PROGMEM LED_CMD[] = "led";
const char PROGMEM GPS_DATA_CMD[] = "gpsdata";
const char PROGMEM GPS_CMD[] = "gps";
const char PROGMEM NMEA_CMD[] = "nmea";

// WiFi sniff/scan
const char PROGMEM EVIL_PORTAL_CMD[] = "evilportal";
const char PROGMEM SIGSTREN_CMD[] = "sigmon";
const char PROGMEM SCANAP_CMD[] = "scanap";
const char PROGMEM SCANSTA_CMD[] = "scansta";
const char PROGMEM SNIFF_RAW_CMD[] = "sniffraw";
const char PROGMEM SNIFF_BEACON_CMD[] = "sniffbeacon";
const char PROGMEM SNIFF_PROBE_CMD[] = "sniffprobe";
const char PROGMEM SNIFF_PWN_CMD[] = "sniffpwn";
const char PROGMEM SNIFF_ESP_CMD[] = "sniffesp";
const char PROGMEM SNIFF_DEAUTH_CMD[] = "sniffdeauth";
const char PROGMEM SNIFF_PMKID_CMD[] = "sniffpmkid";
const char PROGMEM STOPSCAN_CMD[] = "stopscan";
const char PROGMEM WARDRIVE_CMD[] = "wardrive";

// WiFi attack
const char PROGMEM ATTACK_CMD[] = "attack";
const char PROGMEM ATTACK_TYPE_DEAUTH[] = "deauth";
const char PROGMEM ATTACK_TYPE_BEACON[] = "beacon";
const char PROGMEM ATTACK_TYPE_PROBE[] = "probe";
const char PROGMEM ATTACK_TYPE_RR[] = "rickroll";

// WiFi Aux
const char PROGMEM LIST_AP_CMD[] = "list";
const char PROGMEM SEL_CMD[] = "select";
const char PROGMEM SSID_CMD[] = "ssid";

// Bluetooth sniff/scan
const char PROGMEM BT_SPAM_CMD[] = "blespam";
const char PROGMEM BT_SNIFF_CMD[] = "sniffbt";
//const char PROGMEM BT_SOUR_APPLE_CMD[] = "sourapple";
//const char PROGMEM BT_SWIFTPAIR_SPAM_CMD[] = "swiftpair";
//const char PROGMEM BT_SAMSUNG_SPAM_CMD[] = "samsungblespam";
//onst char PROGMEM BT_SPAM_ALL_CMD[] = "btspamall";
const char PROGMEM BT_WARDRIVE_CMD[] = "btwardrive";
const char PROGMEM BT_SKIM_CMD[] = "sniffskim";


//// Command help messages
// Admin
const char PROGMEM HELP_HEAD[] = "============ Commands ============";
const char PROGMEM HELP_CH_CMD[] = "channel [-s <channel>]";
const char PROGMEM HELP_CLEARAP_CMD_A[] = "clearlist -a/-c/-s";
const char PROGMEM HELP_REBOOT_CMD[] = "reboot";
const char PROGMEM HELP_UPDATE_CMD_A[] = "update -s/-w";
const char PROGMEM HELP_SETTINGS_CMD[] = "settings [-s <setting> enable/disable>]/[-r]";
const char PROGMEM HELP_LS_CMD[] = "ls <directory>";
const char PROGMEM HELP_LED_CMD[] = "led -s <hex color>/-p <rainbow>";
const char PROGMEM HELP_GPS_DATA_CMD[] = "gpsdata";
const char PROGMEM HELP_GPS_CMD[] = "gps [-g] <fix/sat/lon/lat/alt/date/accuracy/text/nmea>\r\n    [-n] <native/all/gps/glonass/galileo/navic/qzss/beidou>\r\n         [-b = use BD vs GB for beidou]";
const char PROGMEM HELP_NMEA_CMD[] = "nmea";

// WiFi sniff/scan
const char PROGMEM HELP_EVIL_PORTAL_CMD[] = "evilportal [-c start [-w html.html]/sethtml <html.html>]";
const char PROGMEM HELP_SIGSTREN_CMD[] = "sigmon";
const char PROGMEM HELP_SCANAP_CMD[] = "scanap";
const char PROGMEM HELP_SCANSTA_CMD[] = "scansta";
const char PROGMEM HELP_SNIFF_RAW_CMD[] = "sniffraw";
const char PROGMEM HELP_SNIFF_BEACON_CMD[] = "sniffbeacon";
const char PROGMEM HELP_SNIFF_PROBE_CMD[] = "sniffprobe";
const char PROGMEM HELP_SNIFF_PWN_CMD[] = "sniffpwn";
const char PROGMEM HELP_SNIFF_ESP_CMD[] = "sniffesp";
const char PROGMEM HELP_SNIFF_DEAUTH_CMD[] = "sniffdeauth";
const char PROGMEM HELP_SNIFF_PMKID_CMD[] = "sniffpmkid [-c <channel>][-d][-l]";
const char PROGMEM HELP_STOPSCAN_CMD[] = "stopscan";
const char PROGMEM HELP_WARDRIVE_CMD[] = "wardrive [-s]";

// WiFi attack
const char PROGMEM HELP_ATTACK_CMD[] = "attack -t <beacon [-l/-r/-a]/deauth [-c]/[-s <src mac>] [-d <dst mac>]/probe/rickroll>";

// WiFi Aux
const char PROGMEM HELP_LIST_AP_CMD_A[] = "list -s";
const char PROGMEM HELP_LIST_AP_CMD_B[] = "list -a";
const char PROGMEM HELP_LIST_AP_CMD_C[] = "list -c";
const char PROGMEM HELP_SEL_CMD_A[] = "select -a/-s/-c <index (comma separated)>/-f \"equals <String> or contains <String>\"";
const char PROGMEM HELP_SSID_CMD_A[] = "ssid -a [-g <count>/-n <name>]";
const char PROGMEM HELP_SSID_CMD_B[] = "ssid -r <index>";

// Bluetooth sniff/scan
const char PROGMEM HELP_BT_SNIFF_CMD[] = "sniffbt";
const char PROGMEM HELP_BT_SPAM_CMD[] = "blespam -t <apple/google/samsung/windows/all>";
//const char PROGMEM HELP_BT_SOUR_APPLE_CMD[] = "sourapple";
//const char PROGMEM HELP_BT_SWIFTPAIR_SPAM_CMD[] = "swiftpair";
//const char PROGMEM HELP_BT_SAMSUNG_SPAM_CMD[] = "samsungblespam";
//onst char PROGMEM HELP_BT_SPAM_ALL_CMD[] = "btspamall";
const char PROGMEM HELP_BT_WARDRIVE_CMD[] = "btwardrive [-c]";
const char PROGMEM HELP_BT_SKIM_CMD[] = "sniffskim";
const char PROGMEM HELP_FOOT[] = "==================================";


class CommandLine {
  private:
    String getSerialInput();
    LinkedList<String> parseCommand(String input, char* delim);
    String toLowerCase(String str);
    void filterAccessPoints(String filter);
    void runCommand(String input);
    bool checkValueExists(LinkedList<String>* cmd_args_list, int index);
    bool inRange(int max, int index);
    bool apSelected();
    bool hasSSIDs();
    void showCounts(int selected, int unselected = -1);
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
