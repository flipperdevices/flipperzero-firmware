/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: No OTA
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

#include "Display.h"
#include "WiFiScan.h"
#include "MenuFunctions.h"

Display display_obj;
WiFiScan wifi_scan_obj;
MenuFunctions menu_function_obj;

uint32_t currentTime  = 0;

void setup()
{

  pinMode(FLASH_BUTTON, INPUT);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
  
  Serial.begin(115200);
  Serial.println("\n\n--------------------------------");
  Serial.println("         ESP32 Marauder      ");
  Serial.println("--------------------------------\n\n");

  // Run display setup
  display_obj.RunSetup();

  // Build menus
  menu_function_obj.RunSetup();
}


void loop()
{
  // get the current time
  currentTime = millis();

  // Update all of our objects
  display_obj.main(); 
  wifi_scan_obj.main(currentTime);
  menu_function_obj.main();

  delay(1);
}
