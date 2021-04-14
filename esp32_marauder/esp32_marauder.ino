/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: Minimal SPIFFS
https://www.online-utility.org/image/convert/to/XBM
*/

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
#include "Display.h"
#include "WiFiScan.h"
#include "MenuFunctions.h"
#include "SDInterface.h"
#include "Web.h"
#include "Buffer.h"
#include "BatteryInterface.h"
#include "TemperatureInterface.h"
#include "LedInterface.h"
#include "esp_interface.h"
#include "a32u4_interface.h"
//#include "icons.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
*/

Display display_obj;
WiFiScan wifi_scan_obj;
MenuFunctions menu_function_obj;
SDInterface sd_obj;
Web web_obj;
Buffer buffer_obj;
BatteryInterface battery_obj;
TemperatureInterface temp_obj;
LedInterface led_obj;
EspInterface esp_obj;
A32u4Interface a32u4_obj;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

uint32_t currentTime  = 0;

void setup()
{
  pinMode(FLASH_BUTTON, INPUT);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
#if BATTERY_ANALOG_ON == 1
  pinMode(BATTERY_PIN, OUTPUT);
  pinMode(CHARGING_PIN, INPUT);
#endif
  
  // Preset SPI CS pins to avoid bus conflicts
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);

  Serial.begin(115200);
  
  //Serial.begin(115200);
  
  display_obj.RunSetup();
  display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);

  digitalWrite(TFT_BL, LOW);

  // Draw the title screen
  display_obj.drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image

  //showCenterText(version_number, 250);
  display_obj.tft.drawCentreString(display_obj.version_number, 120, 250, 2);

  digitalWrite(TFT_BL, HIGH); // Need this

  delay(2000);

  display_obj.clearScreen();

  display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

  display_obj.tft.println("Giving room for HardwareSerial...");

  delay(2000);

  display_obj.tft.println("Marauder " + display_obj.version_number + "\n");

  display_obj.tft.println("Started Serial");
  
  Serial.println(F("\n\n--------------------------------\n"));
  Serial.println(F("         ESP32 Marauder      \n"));
  Serial.println("            " + display_obj.version_number + "\n");
  Serial.println(F("       By: justcallmekoko\n"));
  Serial.println(F("--------------------------------\n\n"));

  //Serial.println("Internal Temp: " + (String)((temprature_sens_read() - 32) / 1.8));

  wifi_scan_obj.RunSetup();

  Serial.println(wifi_scan_obj.freeRAM());

  display_obj.tft.println("Checked RAM");

  // Do some SD stuff
  if(sd_obj.initSD()) {
    Serial.println(F("SD Card supported"));
    display_obj.tft.println(F("Initialized SD Card"));
  }
  else {
    Serial.println(F("SD Card NOT Supported"));
    display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
    display_obj.tft.println(F("Failed to Initialize SD Card"));
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
  }

  // Run display setup
  Serial.println(wifi_scan_obj.freeRAM());
  //display_obj.RunSetup();

  // Build menus
  Serial.println(wifi_scan_obj.freeRAM());
  //menu_function_obj.RunSetup();

  //display_obj.tft.println("Created Menu Structure");

  // Battery stuff
  Serial.println(wifi_scan_obj.freeRAM());
  battery_obj.RunSetup();

  display_obj.tft.println(F("Checked battery configuration"));

  // Temperature stuff
  Serial.println(wifi_scan_obj.freeRAM());
  temp_obj.RunSetup();

  display_obj.tft.println(F("Initialized temperature interface"));

  battery_obj.battery_level = battery_obj.getBatteryLevel();

  if (battery_obj.i2c_supported) {
    Serial.println(F("IP5306 I2C Supported: true"));
  }
  else
    Serial.println(F("IP5306 I2C Supported: false"));

  Serial.println(wifi_scan_obj.freeRAM());

  // Do some LED stuff
  led_obj.RunSetup();

  display_obj.tft.println(F("Initialized LED Interface"));

  //display_obj.tft.println(F("Starting..."));

  delay(500);

  //display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // OG Logo Section
  /*
  digitalWrite(TFT_BL, LOW);

  // Draw the title screen
  display_obj.drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image

  //showCenterText(version_number, 250);
  display_obj.tft.drawCentreString(display_obj.version_number, 120, 250, 2);

  digitalWrite(TFT_BL, HIGH);
  */

  esp_obj.begin();
  
  a32u4_obj.begin(); // This goes last to make sure nothing is messed up when reading serial

  display_obj.tft.println(F("Starting..."));

  display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);

  delay(2000);

  menu_function_obj.RunSetup();
}


void loop()
{
  // get the current time
  //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
  currentTime = millis();

  // Update all of our objects
  //if ((!display_obj.draw_tft) &&
  //    (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  if ((!display_obj.draw_tft) && (wifi_scan_obj.currentScanMode != ESP_UPDATE))
  {
    display_obj.main(wifi_scan_obj.currentScanMode);
    wifi_scan_obj.main(currentTime);
    sd_obj.main();
    battery_obj.main(currentTime);
    temp_obj.main(currentTime);
    esp_obj.main(currentTime);
    a32u4_obj.main(currentTime);
    //led_obj.main(currentTime);
    //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
    if ((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL))
      menu_function_obj.main(currentTime);
      if (wifi_scan_obj.currentScanMode == OTA_UPDATE)
        web_obj.main();
    delay(1);
  }
  else if ((display_obj.draw_tft) &&
           (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  {
    display_obj.drawStylus();
  }
  else if (wifi_scan_obj.currentScanMode == ESP_UPDATE) {
    display_obj.main(wifi_scan_obj.currentScanMode);
    menu_function_obj.main(currentTime);
    esp_obj.program();
    delay(1);
  }
  //else
  //{
  //  web_obj.main();
  //}

  //Serial.println(wifi_scan_obj.currentScanMode);

  //Serial.print("Run Time: ");
  //Serial.print(millis() - currentTime);
  //Serial.println("ms");
}
