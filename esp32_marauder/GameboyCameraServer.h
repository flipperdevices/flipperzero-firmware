#ifndef GameBoyCameraServer_h
#define GameBoyCameraServer_h

#include <iostream>
#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "GameboyServer.h"
#include "Gameboy.h"
#include "configs.h"
#include "gameboy_server_link_camera_assets.h"

class GameBoyCameraServer {
  private:

    uint8_t clock_count = 0x00;
    uint8_t current_byte = 0x00;
    uint32_t packet_count = 0x00;
    uint32_t packet_length = 0x00;
    uint8_t current_packet_type = 0x00;
    bool printed = false;
    uint8_t inquiry_count = 0x00;
    uint8_t image_data[11520] = {};
    uint32_t img_index = 0;
    uint8_t last_image_data[11520] = {};
    uint32_t last_img_index = 0;
    String imageData = "";

    unsigned long lastByteReceived = 0;
    unsigned long blinkClockHit = 0;
    bool blinkCycle = false;
    bool successPrinted = false;
    bool webConnected = false;
    bool initializedAttach = false;

    bool runGameBoyCameraServer;


    void setup();
    void processByte(uint8_t data);
    void resetImageData();
    void resetValues();
    void storeData(uint8_t *image_data);
    void espprinter_setup();
    void espprinter_loop();
  public:
    GameBoyCameraServer();
    bool isRunning();
    void main();
    void begin();
    void start();
    void stop();
    void gbClockHit();
    // void gbClockHitWrapper();
    // void onImageRequestWrapper(AsyncWebServerRequest *request);
    // void onResetRequestWrapper(AsyncWebServerRequest *request);
    void onResetRequest(AsyncWebServerRequest *request);
    void onImageRequest(AsyncWebServerRequest *request);
};

#endif