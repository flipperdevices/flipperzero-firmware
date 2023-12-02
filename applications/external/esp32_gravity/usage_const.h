#ifndef ESP_FLIP_CONST_H
#define ESP_FLIP_CONST_H

/* Command usage string - SHORT_* is compressed help text for Flipper */
const char USAGE_BT[] = "Bluetooth Test Module. bluetooth";
const char USAGE_BEACON[] = "Beacon spam attack. Usage: beacon [ RICKROLL | RANDOM [ COUNT ] |\n\t\tINFINITE | TARGET-SSIDs | APs | OFF ] [ AUTH ( OPEN | WPA )+ ]";
const char USAGE_TARGET_SSIDS[] = "Manage SSID targets. Usage: target-ssids [ ( ADD | REMOVE ) <ssid_name> ]";
const char USAGE_PROBE[] = "Probe flood attack. Usage: probe [ ANY | TARGET-SSIDs | APs | OFF ]";
const char USAGE_FUZZ[] = "Various invalid packets. Usage: fuzz OFF | ( ( BEACON | REQ | RESP )+\n\t\t( OVERFLOW | MALFORMED ) [ BROADCAST | TARGET-SSIDs | selectedSTA | selectedAP | RANDOM ] )";
const char USAGE_SNIFF[] = "Display interesting packets. Usage: sniff [ ON | OFF ]";
const char USAGE_DEAUTH[] = "Deauth attack. Usage: deauth [ <millis> ] [ FRAME | DEVICE | SPOOF ]\n\t\t[ STA | AP | BROADCAST | OFF ]";
const char USAGE_MANA[] = "Mana attack. Usage: mana ( CLEAR | ( [ VERBOSE ] [ ON | OFF ] ) |\n\t\t( AUTH [ NONE | WEP | WPA ] ) | ( LOUD [ ON | OFF ] ) )";
const char USAGE_STALK[] = "Toggle target tracking/homing. Usage: stalk [ ON | OFF ]";
const char USAGE_AP_DOS[] = "Denial-of-service attack on selectedAPs. Usage: ap-dos [ ON | OFF ]";
const char USAGE_AP_CLONE[] = "Clone and attempt takeover of the specified AP.\n\tUsage: ap-clone [ ( ON | OFF ) ( OPEN | WEP | WPA )+ ]";
const char USAGE_SCAN[] = "scan [ ( [ <ssid> ] WIFI ) | BT [ SERVICES ] |\n\t\tBLE [ PURGE ( RSSI [ <maxRSSI> ] | AGE [ <minAge> ] |\n\t\tUNNAMED | UNSELECTED | NONE )+ ] | OFF ]";
const char USAGE_HOP[] = "Configure channel hopping. Usage: hop [ <millis> ] [ ON | OFF | DEFAULT | KILL ]\n\t\t[ SEQUENTIAL | RANDOM ]";
const char USAGE_SET[] = "Set a variable. Usage: set <variable> <value>";
const char USAGE_GET[] = "Get a variable. Usage: get <variable>";
const char USAGE_VIEW[] = "List available targets. Usage: view ( ( AP [ selectedSTA ] ) |\n\t\t( STA [ selectedAP ] ) | BT [ SERVICES [ SELECTED | KNOWN | UNKNOWN ] ] |\n\t\tSORT ( AGE | RSSI | SSID ) )+";
const char USAGE_SELECT[] = "Select an element. Usage: select ( AP | STA | BT ) <elementId>+";
const char USAGE_SELECTED[] = "Display selected elements. Usage: selected ( AP | STA | BT )";
const char USAGE_CLEAR[] = "Clear stored APs, STAs or HCIs. Usage: clear ( AP [ SELECTED ] | STA [ SELECTED ] | BT [ SERVICES | SELECTED ] | ALL )";
const char USAGE_HANDSHAKE[] = "Toggle monitoring for encryption material. Usage handshake [ ON | OFF ]";
const char USAGE_COMMANDS[] = "Display a *brief* summary of Gravity commands";
const char USAGE_INFO[] = "Provide help information for the specified command. Usage: info <cmd>";
const char USAGE_VERSION[] = "Display esp32-Gravity version information. Usage: gravity-version";
const char USAGE_BT_STRAT[] = "BLE Purge Strategy. Permitted values: RSSI AGE UNNAMED UNSELECTED NONE.\n\t\tAlternatively can be specified by providing a total value where\n\t\tRSSI is 1, AGE 2, UNNAMED 4, UNSELECTED 8, and NONE 16.";
const char USAGE_PURGE[] = "Purge cached devices based on criteria. Usage: purge [ AP | STA | BT | BLE ]+\n\t\t[ RSSI [ <maxRSSI> ] | AGE [ <minAge> ] | UNNAMED | UNSELECTED | NONE ]+";

const char SHORT_PURGE[] = "purge [ AP | STA | BT | BLE ]+ [ RSSI [ <maxRSSI> ] | AGE [ <minAge> ] | UNNAMED | UNSELECTED | NONE ]+";
const char SHORT_BT_STRAT[] = "set BLE_PURGE_STRAT ( RSSI | AGE | UNNAMED | UNSELECTED | NONE )+";
const char SHORT_BT[] = "bluetooth [ ON | OFF ]";
const char SHORT_BEACON[] = "beacon [ RICKROLL | RANDOM [ COUNT ] | INFINITE | TARGET-SSIDs | APs | OFF] [ AUTH ( OPEN | WPA )+ ]";
const char SHORT_TARGET_SSIDS[] = "(ADD | REMOVE) <apName>";
const char SHORT_PROBE[] = "probe ANY | TARGET-SSIDs | APs | OFF";
const char SHORT_FUZZ[] = "fuzz OFF | ( ( BEACON | REQ | RESP )+ ( OVERFLOW | MALFORMED ) [ BROADCAST | TARGET-SSIDs | selectedSTA | selectedAP | RANDOM ] )";
const char SHORT_SNIFF[] = "sniff [ ON | OFF ]";
const char SHORT_DEAUTH[] = "deauth <millis>";
const char SHORT_MANA[] = "Mana attack";
const char SHORT_STALK[] = "Track RSSI. stalk [ ON | OFF ]";
const char SHORT_AP_DOS[] = "ap-dos [ ON | OFF ]";
const char SHORT_AP_CLONE[] = "ap-clone [ ( ON | OFF ) ( OPEN | WEP | WPA )+ ]";
const char SHORT_SCAN[] = "scan [ ( [ <ssid> ] WIFI ) | BT [ SERVICES ] | BLE [ PURGE ( RSSI [ <maxRSSI> ] | AGE [ <minAge> ] | UNNAMED | UNSELECTED | NONE )+ ] | OFF ]";
const char SHORT_HOP[] = "hop [ <millis> ] [ ON | OFF | DEFAULT | KILL ] [ SEQUENTIAL | RANDOM ]";
const char SHORT_SET[] = "set <variable> <value>";
const char SHORT_GET[] = "get <variable>";
const char SHORT_VIEW[] = "VIEW ( ( AP [ selectedSTA ] ) | ( STA [ selectedAP ] ) | BT [ SERVICES [ SELECTED | KNOWN | UNKNOWN ] ] | SORT ( AGE | RSSI | SSID ) )+";
char SHORT_SELECT[] = "select ( AP | STA | BT ) <id>+ sep. ~";
const char SHORT_SELECTED[] = "selected ( AP | STA | BT )";
const char SHORT_CLEAR[] = "clear ( AP [ SELECTED ] | STA [ SELECTED ] | BT [ SERVICES | SELECTED ] | ALL )";
const char SHORT_HANDSHAKE[] = "handshake [ ON | OFF ]";
const char SHORT_COMMANDS[] = "Brief command summary";
const char SHORT_INFO[] = "Command help. info <cmd>";
const char SHORT_VERSION[] = "esp32-Gravity Version. gravity-version";

#endif