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
//#include "Web.h"
#include "EvilPortal.h"
#include <Wire.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <Arduino.h>

#ifdef HAS_GPS
  #include "GpsInterface.h"
#endif

#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool camera_initialized = false;
#include "Assets.h"
#include "WiFiScan.h"
#ifdef HAS_SD
  #include "SDInterface.h"
#endif
#include "Buffer.h"

#ifdef MARAUDER_FLIPPER
  #include "flipperLED.h"
#elif defined(XIAO_ESP32_S3)
  #include "xiaoLED.h"
#else
  #include "LedInterface.h"
#endif

//#include "esp_interface.h"
#include "settings.h"
#include "CommandLine.h"
#include "lang_var.h"

#ifdef HAS_BATTERY
  #include "BatteryInterface.h"
#endif

//#ifdef HAS_TEMP_SENSOR
//  #include "TemperatureInterface.h"
//#endif

#ifdef HAS_SCREEN
  #include "Display.h"
  #include "MenuFunctions.h"
  //#include "a32u4_interface.h"
#endif

#ifdef HAS_BUTTONS
  #include <SwitchLib.h>
  
  #if (U_BTN >= 0)
    SwitchLib u_btn = SwitchLib(U_BTN, 1000, true);
  #endif
  #if (D_BTN >= 0)
    SwitchLib d_btn = SwitchLib(D_BTN, 1000, true);
  #endif
  #if (L_BTN >= 0)
    SwitchLib l_btn = SwitchLib(L_BTN, 1000, true);
  #endif
  #if (R_BTN >= 0)
    SwitchLib r_btn = SwitchLib(R_BTN, 1000, true);
  #endif
  #if (C_BTN >= 0)
    SwitchLib c_btn = SwitchLib(C_BTN, 1000, true);
  #endif

#endif

WiFiScan wifi_scan_obj;
EvilPortal evil_portal_obj;
//Web web_obj;
Buffer buffer_obj;
//EspInterface esp_obj;
Settings settings_obj;
CommandLine cli_obj;

#ifdef HAS_GPS
  GpsInterface gps_obj;
#endif

#ifdef HAS_BATTERY
  BatteryInterface battery_obj;
#endif

//#ifdef HAS_TEMP_SENSOR
//  TemperatureInterface temp_obj;
//#endif

#ifdef HAS_SCREEN
  Display display_obj;
  MenuFunctions menu_function_obj;
  //A32u4Interface a32u4_obj;
#endif

#ifdef HAS_SD
  SDInterface sd_obj;
#endif

#ifdef MARAUDER_M5STICKC
  AXP192 axp192_obj;
#endif

#ifdef MARAUDER_FLIPPER
  flipperLED flipper_led;
#elif defined(XIAO_ESP32_S3)
  xiaoLED xiao_led;
#else
  LedInterface led_obj;
#endif

const String PROGMEM version_number = MARAUDER_VERSION;

#ifdef HAS_NEOPIXEL_LED
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);
#endif

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
  #ifdef MARAUDER_M5STICKC
    axp192_obj.begin();
  #endif
  
  // Start with the flashlight off
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(230400);

  unsigned long waitForStreamMode = millis() + 3000;

  while (waitForStreamMode > millis()) {
    if (Serial.available())  // if we receive anything, just switch to another mode
    {
      switch (Serial.read()) {
        case 'q':  // QR code reader mode
          qr_reader_setup();
          for (;;)
            qr_reader_loop();

        case 'm':  // Motion detection
          motion_detection_setup();
          for (;;)
            motion_detection_loop();

        case 'c':  // Camera stream
          cam_stream_setup();
          for (;;)
            cam_stream_loop();

        case 'n':  // Nanny cam
          nanny_cam_setup();
          for (;;)
            nanny_cam_loop();

        case '.':  // Morse flasher
          morse_setup();
          for (;;)
            morse_loop();

        /*case 'e':  // Evil portal
          evilportal_setup();
          for (;;)
            evilportal_loop();*/

        case 'w':  // Marauder
          goto continue_to_marauder;
      }
    }
  }
  continue_to_marauder:;

  //pinMode(FLASH_BUTTON, INPUT);

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
  
  /*#ifdef HAS_SD
    pinMode(SD_CS, OUTPUT);

    delay(10);
  
    digitalWrite(SD_CS, HIGH);

    delay(10);
  #endif

  Serial.begin(115200);*/

  // Starts a second serial channel to stream the captured packets
  #ifdef WRITE_PACKETS_SERIAL
    
    #ifdef XIAO_ESP32_S3
      Serial1.begin(115200, SERIAL_8N1, XIAO_RX1, XIAO_TX1);
    #else
      Serial1.begin(115200);
    #endif
    
  #endif

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
    #ifndef MARAUDER_MINI
      display_obj.drawJpeg("/marauder3L.jpg", 0 , 0);     // 240 x 320 image
    #else
      display_obj.drawJpeg("/marauder3L.jpg", 0, 0);
    #endif
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

  #ifdef WRITE_PACKETS_SERIAL
    buffer_obj = Buffer();
  #elif defined(HAS_SD)
    // Do some SD stuff
    if(sd_obj.initSD()) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(F(text_table0[3]));
      #endif
    } else {
      Serial.println(F("SD Card NOT Supported"));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
        display_obj.tft.println(F(text_table0[4]));
        display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
      #endif
    }
  #else
    return;
  #endif

  #ifdef HAS_BATTERY
    battery_obj.RunSetup();
  #endif
  
  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[5]));
  #endif

  // Temperature stuff
  //#ifdef HAS_TEMP_SENSOR
  //  temp_obj.RunSetup();
  //#endif

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[6]));
  #endif

  #ifdef HAS_BATTERY
    battery_obj.battery_level = battery_obj.getBatteryLevel();
  
//    if (battery_obj.i2c_supported) {
//      Serial.println(F("IP5306 I2C Supported: true"));
//    }
//    else
//      Serial.println(F("IP5306 I2C Supported: false"));
  #endif

  // Do some LED stuff
  #ifdef MARAUDER_FLIPPER
    flipper_led.RunSetup();
  #elif defined(XIAO_ESP32_S3)
    xiao_led.RunSetup();
  #else
    led_obj.RunSetup();
  #endif

  #ifdef HAS_SCREEN
    display_obj.tft.println(F(text_table0[7]));

    delay(500);
  #endif

  #ifdef HAS_GPS
    gps_obj.begin();
    #ifdef HAS_SCREEN
      if (gps_obj.getGpsModuleStatus())
        display_obj.tft.println("GPS Module connected");
      else
        display_obj.tft.println("GPS Module NOT connected");
    #endif
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
  
  Serial.println(F("CLI Ready"));
  cli_obj.RunSetup();
}


void loop()
{
  currentTime = millis();
  bool mini = false;

  #ifdef MARAUDER_MINI
    mini = true;
  #endif

  // Update all of our objects
  /*#ifdef HAS_SCREEN
    bool do_draw = display_obj.draw_tft;
  #else
    bool do_draw = false;
  #endif*/
  
  //if ((!do_draw) && (wifi_scan_obj.currentScanMode != ESP_UPDATE))
  //{
  cli_obj.main(currentTime);
  #ifdef HAS_SCREEN
    display_obj.main(wifi_scan_obj.currentScanMode);
  #endif
  wifi_scan_obj.main(currentTime);
  //evil_portal_obj.main(wifi_scan_obj.currentScanMode);

  #ifdef HAS_GPS
    gps_obj.main();
  #endif
  
  #ifdef WRITE_PACKETS_SERIAL
    buffer_obj.forceSaveSerial();
  #elif defined(HAS_SD)
    sd_obj.main();
  #else
    return;
  #endif

  #ifdef HAS_BATTERY
    battery_obj.main(currentTime);
    //temp_obj.main(currentTime);
  #endif
  settings_obj.main(currentTime);
  if (((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) && (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL)) ||
      (mini)) {
    #ifdef HAS_SCREEN
      menu_function_obj.main(currentTime);
    #endif
    //cli_obj.main(currentTime);
  }
  #ifdef MARAUDER_FLIPPER
    flipper_led.main();
  #elif defined(XIAO_ESP32_S3)
    xiao_led.main();
  #else
    led_obj.main(currentTime);
  #endif

  //if (wifi_scan_obj.currentScanMode == OTA_UPDATE)
  //  web_obj.main();
  #ifdef HAS_SCREEN
    delay(1);
  #else
    delay(50);
  #endif
  //}
  /*else if (wifi_scan_obj.currentScanMode == ESP_UPDATE) {
    #ifdef HAS_SCREEN
      display_obj.main(wifi_scan_obj.currentScanMode);
      menu_function_obj.main(currentTime);
    #endif

    #ifdef MARAUDER_FLIPPER
      flipper_led.main();
    #elif defined(XIAO_ESP32_S3)
      xiao_led.main();
    #else
      led_obj.main(currentTime);
    #endif
    
    //cli_obj.main(currentTime);
    delay(1);
  }*/
}
