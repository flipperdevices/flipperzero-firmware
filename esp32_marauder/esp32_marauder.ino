/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: Minimal SPIFFS
https://www.online-utility.org/image/convert/to/XBM
*/

#include "configs.h"

#ifndef HAS_SCREEN
  #define MenuFunctions_h
  #define Display_h
#endif

#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <Arduino.h>

#include "Assets.h"
#include "WiFiScan.h"
#include "SDInterface.h"
#include "Web.h"
#include "Buffer.h"
#include "BatteryInterface.h"
#include "TemperatureInterface.h"
#include "LedInterface.h"
#include "esp_interface.h"
#include "settings.h"
#include "CommandLine.h"
#include "lang_var.h"

#ifdef HAS_SCREEN
  #include "Display.h"
  #include "MenuFunctions.h"
  #include "a32u4_interface.h"
#endif

#ifdef MARAUDER_MINI
  #include <SwitchLib.h>
  SwitchLib u_btn = SwitchLib(U_BTN, 1000, true);
  SwitchLib d_btn = SwitchLib(D_BTN, 1000, true);
  SwitchLib l_btn = SwitchLib(L_BTN, 1000, true);
  SwitchLib r_btn = SwitchLib(R_BTN, 1000, true);
  SwitchLib c_btn = SwitchLib(C_BTN, 1000, true);
#endif

WiFiScan wifi_scan_obj;
SDInterface sd_obj;
Web web_obj;
Buffer buffer_obj;
BatteryInterface battery_obj;
TemperatureInterface temp_obj;
LedInterface led_obj;
EspInterface esp_obj;
Settings settings_obj;
CommandLine cli_obj;

#ifdef HAS_SCREEN
  Display display_obj;
  MenuFunctions menu_function_obj;
  A32u4Interface a32u4_obj;
#endif

const String PROGMEM version_number = MARAUDER_VERSION;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

uint32_t currentTime  = 0;


void backlightOn() {
  #ifdef HAS_SCREEN
    #ifdef MARAUDER_MINI
      digitalWrite(TFT_BL, LOW);
    #endif
  
    #ifndef MARAUDER_MINI
      digitalWrite(TFT_BL, HIGH);
    #endif
  #endif
}

void backlightOff() {
  #ifdef HAS_SCREEN
    #ifdef MARAUDER_MINI
      digitalWrite(TFT_BL, HIGH);
    #endif
  
    #ifndef MARAUDER_MINI
      digitalWrite(TFT_BL, LOW);
    #endif
  #endif
}


void setup()
{
  pinMode(FLASH_BUTTON, INPUT);

  #ifdef HAS_SCREEN
    pinMode(TFT_BL, OUTPUT);
  #endif
  
  backlightOff();
#if BATTERY_ANALOG_ON == 1
  pinMode(BATTERY_PIN, OUTPUT);
  pinMode(CHARGING_PIN, INPUT);
#endif
  
  // Preset SPI CS pins to avoid bus conflicts
  #ifdef HAS_SCREEN
    digitalWrite(TFT_CS, HIGH);
  #endif
  
  digitalWrite(SD_CS, HIGH);

  Serial.begin(115200);
  
  //Serial.begin(115200);

  //Serial.println("\n\nHello, World!\n");

  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));

  //#ifdef HAS_SCREEN
  //  Serial.println("Has Screen");
  //#else
  //  Serial.println("Does not have screen");
  //#endif

  #ifdef HAS_SCREEN
    display_obj.RunSetup();
    display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
  #endif

  backlightOff();

  // Draw the title screen
  #ifdef HAS_SCREEN
    display_obj.drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image
  #endif

  #ifdef HAS_SCREEN
    //showCenterText(version_number, 250);
    #ifndef MARAUDER_MINI
      display_obj.tft.drawCentreString(display_obj.version_number, 120, 250, 2);
    #endif
  
    #ifdef MARAUDER_MINI
      display_obj.tft.drawCentreString(display_obj.version_number, TFT_WIDTH/2, TFT_HEIGHT, 1);
    #endif
  #endif

  backlightOn(); // Need this

  #ifdef HAS_SCREEN
    delay(2000);

    display_obj.clearScreen();
  
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
  
    display_obj.tft.println(text_table0[0]);
  
    delay(2000);
  
    display_obj.tft.println("Marauder " + display_obj.version_number + "\n");
  
    display_obj.tft.println(text_table0[1]);
  #endif

  //Serial.println("Internal Temp: " + (String)((temprature_sens_read() - 32) / 1.8));

  settings_obj.begin();

  //Serial.println("This is a test Channel: " + (String)settings_obj.loadSetting<uint8_t>("Channel"));
  //if (settings_obj.loadSetting<bool>( "Force PMKID"))
  //  Serial.println("This is a test Force PMKID: true");
  //else
  //  Serial.println("This is a test Force PMKID: false");

  wifi_scan_obj.RunSetup();

  //Serial.println(wifi_scan_obj.freeRAM());

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[2]));
  #endif

  // Do some SD stuff
  if(sd_obj.initSD()) {
    //Serial.println(F("SD Card supported"));
    #ifdef HAS_SCREEN
      display_obj.tft.println(F(text_table0[3]));
    #endif
  }
  else {
    Serial.println(F("SD Card NOT Supported"));
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
      display_obj.tft.println(F(text_table0[4]));
      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    #endif
  }

  battery_obj.RunSetup();

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[5]));
  #endif

  // Temperature stuff
  #ifndef MARAUDER_FLIPPER
    temp_obj.RunSetup();
  #endif

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[6]));
  #endif

  #ifndef MARAUDER_FLIPPER
    battery_obj.battery_level = battery_obj.getBatteryLevel();
  
//    if (battery_obj.i2c_supported) {
//      Serial.println(F("IP5306 I2C Supported: true"));
//    }
//    else
//      Serial.println(F("IP5306 I2C Supported: false"));
  #endif

  // Do some LED stuff
  #ifndef MARAUDER_FLIPPER
    led_obj.RunSetup();
  #endif

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[7]));

    delay(500);
  #endif

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[8]));
  
    display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
    delay(2000);
  #endif

  #ifdef HAS_SCREEN
    menu_function_obj.RunSetup();
  #endif

  //Serial.println(F("\n\n--------------------------------\n"));
  //Serial.println(F("         ESP32 Marauder      \n"));
  //Serial.println("            " + version_number + "\n");
  //Serial.println(F("       By: justcallmekoko\n"));
  //Serial.println(F("--------------------------------\n\n"));
  
  Serial.println("CLI Ready");
  cli_obj.RunSetup();
}


void loop()
{
  currentTime = millis();

  // Update all of our objects
  #ifdef HAS_SCREEN
    bool do_draw = display_obj.draw_tft;
  #else
    bool do_draw = false;
  #endif
  
  if ((!do_draw) && (wifi_scan_obj.currentScanMode != ESP_UPDATE))
  {
    cli_obj.main(currentTime);
    #ifdef HAS_SCREEN
      display_obj.main(wifi_scan_obj.currentScanMode);
    #endif
    wifi_scan_obj.main(currentTime);
    sd_obj.main();
    #ifndef MARAUDER_FLIPPER
      battery_obj.main(currentTime);
      temp_obj.main(currentTime);
    #endif
    settings_obj.main(currentTime);
    if ((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL)) {
      #ifdef HAS_SCREEN
        menu_function_obj.main(currentTime);
      #endif
      //cli_obj.main(currentTime);
    }
      if (wifi_scan_obj.currentScanMode == OTA_UPDATE)
        web_obj.main();
    #ifdef HAS_SCREEN
      delay(1);
    #else
      delay(50);
    #endif
  }
  #ifdef HAS_SCREEN
    else if ((display_obj.draw_tft) &&
             (wifi_scan_obj.currentScanMode != OTA_UPDATE))
    {
      display_obj.drawStylus();
    }
  #endif
  else if (wifi_scan_obj.currentScanMode == ESP_UPDATE) {
    #ifdef HAS_SCREEN
      display_obj.main(wifi_scan_obj.currentScanMode);
      menu_function_obj.main(currentTime);
    #endif
    //cli_obj.main(currentTime);
    delay(1);
  }
}
