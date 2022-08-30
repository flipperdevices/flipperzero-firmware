#ifndef lang_var_h
#define lang_var_h


#include "configs.h"

//Starting window texts
PROGMEM const char text0_0[] = "Giving room for HardwareSerial...";
PROGMEM const char text0_1[] = "Started Serial";
PROGMEM const char text0_2[] = "Checked RAM";
PROGMEM const char text0_3[] = "Initialized SD Card";
PROGMEM const char text0_4[] = "Failed to Initialize SD Card";
PROGMEM const char text0_5[] = "Checked battery configuration";
PROGMEM const char text0_6[] = "Initialized temperature interface";
PROGMEM const char text0_7[] = "Initialized LED Interface";
PROGMEM const char text0_8[] = "Starting...";

//Single library (action) texts/Often used
PROGMEM const char text00[] = "Battery Level changed: ";
PROGMEM const char text01[] = "file closed";
PROGMEM const char text02[] = "Failed to open file '";
PROGMEM const char text03[] = "ON";
PROGMEM const char text04[] = "OFF";
PROGMEM const char text05[] = "Load";
PROGMEM const char text06[] = "Save As";
PROGMEM const char text07[] = "Exit";
PROGMEM const char text08[] = "Settings";
PROGMEM const char text09[] = "Back";
PROGMEM const char text10[] = "Channel:";
PROGMEM const char text11[] = "Touch screen to exit";
PROGMEM const char text12[] = "Cancel";
PROGMEM const char text13[] = "Save";
PROGMEM const char text14[] = "Yes";
PROGMEM const char text15[] = "Opening /update.bin...";
PROGMEM const char text16[] = "Close";
PROGMEM const char text17[] = "FAIL";
PROGMEM const char text18[] = "packets/sec: ";


//Menufunctions.cpp texts
PROGMEM const char text1_0[] = "SSID List";
PROGMEM const char text1_1[] = "Add SSIDs";
PROGMEM const char text1_2[] = "SSID: ";
PROGMEM const char text1_3[] = "Password:";
PROGMEM const char text1_4[] = "Setting disabled";
PROGMEM const char text1_5[] = "Setting on";
PROGMEM const char text1_6[] = "ESP32 Marauder ";
PROGMEM const char text1_7[] = "WiFi ";
PROGMEM const char text1_8[] = "Bad USB ";
PROGMEM const char text1_9[] = "Device ";
PROGMEM const char text1_10[] = "General Apps ";
PROGMEM const char text1_11[] = "Updating... ";
PROGMEM const char text1_12[] = "Select Method ";
PROGMEM const char text1_13[] = "Confirm Update ";
PROGMEM const char text1_14[] = "ESP8266 Update ";
PROGMEM const char text1_15[] = "Update Firmware ";
PROGMEM const char text1_16[] = "Language ";
PROGMEM const char text1_17[] = "Device Info ";
PROGMEM const char text1_18[] = "Settings ";
PROGMEM const char text1_19[] = "Bluetooth ";
PROGMEM const char text1_20[] = "WiFi Sniffers ";
PROGMEM const char text1_21[] = "WiFi Attacks ";
PROGMEM const char text1_22[] = "WiFi General ";
PROGMEM const char text1_23[] = "Bluetooth Sniffers ";
PROGMEM const char text1_24[] = "Bluetooth General ";
PROGMEM const char text1_25[] = "Shutdown WiFi ";
PROGMEM const char text1_26[] = "Shutdown BLE ";
PROGMEM const char text1_27[] = "Generate SSIDs ";
PROGMEM const char text1_28[] = "Clear SSIDs ";
PROGMEM const char text1_29[] = "Clear APs ";
PROGMEM const char text1_30[] = "Reboot";
PROGMEM const char text1_31[] = "Sniffers";
PROGMEM const char text1_32[] = "Attacks";
PROGMEM const char text1_33[] = "General";
PROGMEM const char text1_34[] = "Bluetooth Sniffer";
PROGMEM const char text1_35[] = "Detect Card Skimmers";
PROGMEM const char text1_36[] = "Test BadUSB";
PROGMEM const char text1_37[] = "Run Ducky Script";
PROGMEM const char text1_38[] = "Draw";
PROGMEM const char text1_39[] = "Web Update";
PROGMEM const char text1_40[] = "SD Update";
PROGMEM const char text1_41[] = "ESP8266 Update";
PROGMEM const char text1_42[] = "Probe Request Sniff";
PROGMEM const char text1_43[] = "Beacon Sniff";
PROGMEM const char text1_44[] = "Deauth Sniff";
PROGMEM const char text1_45[] = "Packet Monitor";
PROGMEM const char text1_46[] = "EAPOL/PMKID Scan";
PROGMEM const char text1_47[] = "Detect Pwnagotchi";
PROGMEM const char text1_48[] = "Detect Espressif";
PROGMEM const char text1_49[] = "Scan APs";
PROGMEM const char text1_50[] = "Beacon Spam List";
PROGMEM const char text1_51[] = "Beacon Spam Random";
PROGMEM const char text1_52[] = "Rick Roll Beacon";
PROGMEM const char text1_53[] = "Probe Req Flood";
PROGMEM const char text1_54[] = "Deauth Flood";
PROGMEM const char text1_55[] = "Join WiFi";
PROGMEM const char text1_56[] = "Select APs";
PROGMEM const char text1_57[] = "AP Clone Spam";


//SDInterface.cpp texts
PROGMEM const char text2_0[] = "Error, could not find update.bin";
PROGMEM const char text2_1[] = "Starting SD Update...";
PROGMEM const char text2_2[] = "Error, update.bin is empty";
PROGMEM const char text2_3[] = "\nRebooting...\n";
PROGMEM const char text2_4[] = "Could not load update.bin from /";
PROGMEM const char text2_5[] = "File size: ";
PROGMEM const char text2_6[] = "Writing file to partition...";
PROGMEM const char text2_7[] = "Written: ";
PROGMEM const char text2_8[] = "Written only : ";
PROGMEM const char text2_9[] = ". Retry?";
PROGMEM const char text2_10[] = " successfully";
PROGMEM const char text2_11[] = "Update complete";
PROGMEM const char text2_12[] = "Update could not complete";
PROGMEM const char text2_13[] = "Error Occurred. Error #: ";
PROGMEM const char text2_14[] = "Not enough space to begin OTA";

//Web.cpp texts
PROGMEM const char text3_0[] = "Configuring update server...\n\n";
PROGMEM const char text3_1[] = "IP address: ";
PROGMEM const char text3_2[] = "Update: ";
PROGMEM const char text3_3[] = "Bytes complete: ";
PROGMEM const char text3_4[] = "Update Success: ";
PROGMEM const char text3_5[] = "\nCompleted update server setup";

//WiFiScan.cpp texts
PROGMEM const char text4_0[] = " RSSI: ";
PROGMEM const char text4_1[] = "Potential Skimmer: ";
PROGMEM const char text4_2[] = "Already Connected";
PROGMEM const char text4_3[] = "Failed to connect";
PROGMEM const char text4_4[] = "Connected";
PROGMEM const char text4_5[] = "ForcePMKID";
PROGMEM const char text4_6[] = "ForceProbe";
PROGMEM const char text4_7[] = "SavePCAP";
PROGMEM const char text4_8[] = "Probe Flood";
PROGMEM const char text4_9[] = "Clearing APs...";
PROGMEM const char text4_10[] = "APs Cleared: ";
PROGMEM const char text4_11[] = "Clearing SSIDs...";
PROGMEM const char text4_12[] = "SSIDs Cleared: ";
PROGMEM const char text4_13[] = "Generating SSIDs...";
PROGMEM const char text4_14[] = "SSIDs Generated: ";        //Add spaces before to match : [15]
PROGMEM const char text4_15[] = "    Total SSIDs: ";        //Add spaces beforer to match : [14]
PROGMEM const char text4_16[] = "Shutting down WiFi...";
PROGMEM const char text4_17[] = "WiFi not currently initialized";
PROGMEM const char text4_18[] = "Shutting down BLE...";
PROGMEM const char text4_19[] = "BLE not currently initialized";
PROGMEM const char text4_20[] = "Firmware: Marauder";      //From 20 to 35 add spaces so : is in line like it is now
PROGMEM const char text4_21[] = "Version: ";
PROGMEM const char text4_22[] = "ESP-IDF: ";
PROGMEM const char text4_23[] = "WSL Bypass: enabled";
PROGMEM const char text4_24[] = "WSL Bypass: disabled";
PROGMEM const char text4_25[] = "Station MAC: ";
PROGMEM const char text4_26[] = "AP MAC: ";
PROGMEM const char text4_27[] = "";
PROGMEM const char text4_28[] = "SD Card: Connected";
PROGMEM const char text4_29[] = "SD Card Size: ";
PROGMEM const char text4_30[] = "SD Card: Not Connected";
PROGMEM const char text4_31[] = "SD Card Size: 0";
PROGMEM const char text4_32[] = "IP5306 I2C: supported";
PROGMEM const char text4_33[] = "Battery Lvl: ";
PROGMEM const char text4_34[] = "IP5306 I2C: not supported";
PROGMEM const char text4_35[] = "Internal temp: ";
PROGMEM const char text4_36[] = " Detect Espressif ";
PROGMEM const char text4_37[] = " Detect Pwnagotchi ";
PROGMEM const char text4_38[] = " Beacon Sniffer ";
PROGMEM const char text4_39[] = " Deauthentication Sniffer ";
PROGMEM const char text4_40[] = " Probe Request Sniffer ";
PROGMEM const char text4_41[] = " Bluetooth Sniff ";
PROGMEM const char text4_42[] = " Detect Card Skimmers ";
PROGMEM const char text4_43[] = "Scanning for\nBluetooth-enabled skimmers\nHC-03, HC-05, and HC-06...";
PROGMEM const char text4_44[] = " AP Scan ";

//Making tables
PROGMEM const char *text_table0[] = {text0_0,text0_1, text0_2, text0_3, text0_4, text0_5, text0_6, text0_7, text0_8};
PROGMEM const char *text_table1[] = {text1_0,text1_1,text1_2,text1_3,text1_4,text1_5,text1_6,text1_7,text1_8,text1_9,text1_10,text1_11,text1_12,text1_13,text1_14,text1_15,text1_16,text1_17,text1_18,text1_19,text1_20,text1_21,text1_22,text1_23,text1_24,text1_25,text1_26,text1_27,text1_28,text1_29,text1_30,text1_31,text1_32,text1_33,text1_34,text1_35,text1_36,text1_37,text1_38,text1_39,text1_40,text1_41,text1_42,text1_43,text1_44,text1_45,text1_46,text1_47,text1_48,text1_49,text1_50,text1_51,text1_52,text1_53,text1_54,text1_55,text1_56,text1_57};
PROGMEM const char *text_table2[] = {text2_0,text2_1,text2_2,text2_3,text2_4,text2_5,text2_6,text2_7,text2_8,text2_9,text2_10,text2_11,text2_12,text2_13,text2_14};
PROGMEM const char *text_table3[] = {text3_0,text3_1,text3_2,text3_3,text3_4,text3_5};
PROGMEM const char *text_table4[] = {text4_0,text4_1,text4_2,text4_3,text4_4,text4_5,text4_6,text4_7,text1_54,text4_9,text4_10,text4_11,text4_12,text4_13,text4_14,text4_15,text4_16,text4_17,text4_18,text4_19,text4_20,text4_21,text4_22,text4_23,text4_24,text4_25,text4_26,text4_27,text4_28,text4_29,text4_30,text4_31,text4_32,text4_33,text4_34,text4_35,text4_36,text4_37,text4_38,text4_39,text4_40,text4_41,text4_42,text4_43,text4_44};

#endif
