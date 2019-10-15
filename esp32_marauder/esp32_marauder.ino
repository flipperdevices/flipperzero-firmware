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

/*
void getPresses(void *pvParameter)
{
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
   
  while (true)
  {
    boolean pressed = display_obj.tft.getTouch(&t_x, &t_y);
    Serial.print("X: ");
    Serial.print(t_x);
    Serial.print(" Y: ");
    Serial.println(t_y);

    menu_function_obj.pressed = pressed;

    
    //menu_function_obj.handlePress(pressed, t_x, t_y);
    if (pressed)
    {
      menu_function_obj.x = t_x;
      menu_function_obj.y = t_y;
    }

    
    delay(1);
  }
}*/

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
  //menu_function_obj.displayCurrentMenu();

  //xTaskCreate(&getPresses, "getPresses", 2048, NULL, 5, NULL);
  

  // Start a scan mode automatically
  // In the real version, just setup wifi
  // and wait for user input for scan modes
  //wifi_scan_obj.StartScan(WIFI_SCAN_PROBE);

}


void loop()
{
  // get the current time
  currentTime = millis();

  // Update all of our objects
  display_obj.main(); 
  wifi_scan_obj.main(currentTime);
  //if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF)
  menu_function_obj.main();

  //vTaskDelay(portMAX_DELAY);
  delay(1);
}
