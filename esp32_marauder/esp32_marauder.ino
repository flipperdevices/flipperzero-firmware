/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: Minimal SPIFFS
https://www.online-utility.org/image/convert/to/XBM
*/

#include "configs.h"

#include <WiFi.h>
#include "EvilPortal.h"
#include <Wire.h>
#include <SPI.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "driver/rtc_io.h"
#ifdef HAS_GPS
  #include "GpsInterface.h"
#endif

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
#elif defined(MARAUDER_M5STICKC)
  #include "stickcLED.h"
#elif defined(HAS_NEOPIXEL_LED)
  #include "LedInterface.h"
#endif

#include "settings.h"
#include "CommandLine.h"
#include "lang_var.h"


#ifdef HAS_BUTTONS
  #include "Switches.h"
  
  #if (U_BTN >= 0)
    Switches u_btn = Switches(U_BTN, 1000, U_PULL);
  #endif
  #if (D_BTN >= 0)
    Switches d_btn = Switches(D_BTN, 1000, D_PULL);
  #endif
  #if (L_BTN >= 0)
    Switches l_btn = Switches(L_BTN, 1000, L_PULL);
  #endif
  #if (R_BTN >= 0)
    Switches r_btn = Switches(R_BTN, 1000, R_PULL);
  #endif
  #if (C_BTN >= 0)
    Switches c_btn = Switches(C_BTN, 1000, C_PULL);
  #endif

#endif

//  MALVEKE
#include "GameBoyCartridge.h"
#include "GameBoyAdvanceCartridge.h"
#include "GameboyLiveCamera.h"
// #include "GameBoyPhoto.h"
// #include "GameBoyPrinter.h"
#include "uart.h"

WiFiScan wifi_scan_obj;
EvilPortal evil_portal_obj;
Buffer buffer_obj;
Settings settings_obj;
CommandLine cli_obj;

#ifdef HAS_GPS
  GpsInterface gps_obj;
#endif


#ifdef HAS_SD
  SDInterface sd_obj;
#endif


#ifdef MARAUDER_FLIPPER
  flipperLED flipper_led;
#elif defined(XIAO_ESP32_S3)
  xiaoLED xiao_led;
#elif defined(MARAUDER_M5STICKC)
  stickcLED stickc_led;
#else
  LedInterface led_obj;
#endif

const String PROGMEM version_number = MARAUDER_VERSION;

#ifdef HAS_NEOPIXEL_LED
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);
#endif

uint32_t currentTime  = 0;


//  MALVEKE
GameBoyCartridge gameboy_cartridge;
GameBoyAdvanceCartridge gameboy_advance_cartridge;
GameboyLiveCamera gameboy_live_camera;
GameBoyTestPin gameboy_test_pin;
// GameBoyPhoto gameboy_photo;
// GameBoyPrinter gameboy_printer;


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
  Serial.begin(115200);


  unsigned long waitForStreamMode = millis() + 1000;
  while (waitForStreamMode > millis()) {
      if (Serial.available())  // if we receive anything, just switch to another mode
      {
        switch (Serial.read()) {
          #ifdef MALVEKE_VERSION
            case 'c':
              gb_camera_setup();
              for (;;)
                gb_camera_loop();
                break;
          #endif
        }
      }
  }  
  uart_protocol_init();

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 17, 18);
  Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));

  settings_obj.begin();

  //  MALVEKE
  gameboy_cartridge.begin();
  gameboy_advance_cartridge.begin();
  gameboy_live_camera.begin();
  gameboy_test_pin.begin();
  // gameboy_photo.begin();
  // gameboy_printer.begin();
  
  wifi_scan_obj.RunSetup();
  buffer_obj = Buffer();
  evil_portal_obj.setup();

  // #ifdef HAS_GPS
  //   Serial.println("gps_obj begin");
  //   gps_obj.begin();
    
  //   delay(1000);
  // #endif

  
  Serial.println(F("CLI Ready"));
  cli_obj.RunSetup();

  //  Fix led GBCart PIN 31 (ESP32-S2 PIN 9)
  SPI.end();
  periph_module_disable(PERIPH_I2C0_MODULE);
  Wire.end();
}


void loop()
{
  currentTime = millis();
  bool mini = false;

  // Update all of our objects
  /*#ifdef HAS_SCREEN
    bool do_draw = display_obj.draw_tft;
  #else
    bool do_draw = false;
  #endif*/
  
  //if ((!do_draw) && (wifi_scan_obj.currentScanMode != ESP_UPDATE))
  //{
  if(!gameboy_cartridge.isRestoringRAM()){
  cli_obj.main(currentTime);
  }

  gameboy_cartridge.main();
  gameboy_advance_cartridge.main();
  gameboy_live_camera.main();
  gameboy_test_pin.main();
  if(!gameboy_live_camera.isRunning() && !gameboy_cartridge.isWrittingRAM() && !gameboy_cartridge.isWrittingROM() && !gameboy_cartridge.isRestoringRAM()) {

  wifi_scan_obj.main(currentTime);
  //evil_portal_obj.main(wifi_scan_obj.currentScanMode);

  #ifdef HAS_GPS
    gps_obj.main();
  #endif

  // Save buffer to SD and/or serial
  buffer_obj.save();
  settings_obj.main(currentTime);
  if (((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) && (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL)) ||
      (mini)) {
    //cli_obj.main(currentTime);
  }
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
}
