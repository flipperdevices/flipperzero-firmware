#ifndef ESP_FLIP_CONST_H
#define ESP_FLIP_CONST_H

/* Command usage string - SHORT_* is compressed help text for Flipper */
const char USAGE_BEACON[] =
    "Beacon spam attack. Usage: beacon [ RICKROLL | RANDOM [ COUNT ] | INFINITE | TARGET-SSIDs | APs | OFF ]";
const char USAGE_TARGET_SSIDS[] =
    "Manage SSID targets. Usage: target-ssids [ ( ADD | REMOVE ) <ssid_name> ]";
const char USAGE_PROBE[] = "Probe flood attack. Usage: probe [ ANY | TARGET-SSIDs | APs | OFF ]";
const char USAGE_FUZZ[] =
    "Various invalid packets that may (or may not) have interesting effects. Usage: fuzz OFF | ( ( BEACON | REQ | RESP )+ ( OVERFLOW | MALFORMED ) )";
const char USAGE_SNIFF[] = "Display interesting packets. Usage: sniff [ ON | OFF ]";
const char USAGE_DEAUTH[] =
    "Deauth attack. Usage: deauth [ <millis> ] [ FRAME | DEVICE | SPOOF ] [ STA | AP | BROADCAST | OFF ]";
const char USAGE_MANA[] =
    "Mana attack. Usage: mana ( CLEAR | ( [ VERBOSE ] [ ON | OFF ] ) | ( AUTH [ NONE | WEP | WPA ] ) | ( LOUD [ ON | OFF ] ) )";
const char USAGE_STALK[] = "Toggle target tracking/homing. Usage: stalk";
const char USAGE_AP_DOS[] = "802.11 denial-of-service attack. Usage: ap-dos [ ON | OFF ]";
const char USAGE_AP_CLONE[] =
    "Clone and attempt takeover of the specified AP. Usage: ap-clone [ <AP MAC> | APs | OFF ]";
const char USAGE_SCAN[] = "Scan for wireless devices. Usage: scan [ <ssid> | ON | OFF ]";
const char USAGE_HOP[] =
    "Configure channel hopping. Usage: hop [ <millis> ] [ ON | OFF | DEFAULT | KILL ]";
const char USAGE_SET[] = "Set a variable. Usage: set <variable> <value>";
const char USAGE_GET[] = "Get a variable. Usage: get <variable>";
const char USAGE_VIEW[] =
    "List available targets. Usage: view ( ( AP [ selectedSTA ] ) | ( STA [ selectedAP ] ) )+";
const char USAGE_SELECT[] = "Select an element. Usage: select ( AP | STA ) <elementId>+";
const char USAGE_SELECTED[] = "Display selected elements. Usage: selected ( AP | STA )";
const char USAGE_CLEAR[] = "Clear stored APs or STAs. Usage: clear ( AP | STA | ALL )";
const char USAGE_HANDSHAKE[] =
    "Toggle monitoring for encryption material. Usage handshake [ ON | OFF ]";
const char USAGE_COMMANDS[] = "Display a *brief* summary of Gravity commands";
const char USAGE_INFO[] = "Provide help information for the specified command. Usage: info <cmd>";

const char SHORT_BEACON[] = "beacon RANDOM <count>";
const char SHORT_TARGET_SSIDS[] = "(ADD | REMOVE) <apName>";
const char SHORT_PROBE[] = "probe ANY | TARGET-SSIDs | APs | OFF";
const char SHORT_FUZZ[] = "fuzz OFF | ( ( BEACON | REQ | RESP )+ ( OVERFLOW | MALFORMED ) )";
const char SHORT_SNIFF[] = "sniff [ ON | OFF ]";
const char SHORT_DEAUTH[] = "deauth <millis>";
const char SHORT_MANA[] = "Mana attack";
const char SHORT_STALK[] = "Track RSSI";
const char SHORT_AP_DOS[] = "ap-dos [ ON | OFF ]";
const char SHORT_AP_CLONE[] = "ap-clone <AP MAC>";
const char SHORT_SCAN[] = "scan <SSID Name>";
const char SHORT_HOP[] = "hop <millis>";
const char SHORT_SET[] = "set <variable> <value>";
const char SHORT_GET[] = "get <variable>";
const char SHORT_VIEW[] = "VIEW ( ( AP [ selectedSTA ] ) | ( STA [ selectedAP ] ) )+";
const char SHORT_SELECT[] = "select ( AP | STA ) <id>+";
const char SHORT_SELECTED[] = "selected ( AP | STA )";
const char SHORT_CLEAR[] = "clear ( AP | STA | ALL )";
const char SHORT_HANDSHAKE[] = "handshake [ ON | OFF ]";
const char SHORT_COMMANDS[] = "Brief command summary";
const char SHORT_INFO[] = "Command help. info <cmd>";

#endif