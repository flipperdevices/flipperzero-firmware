#include "Web.h"
#include "lang_var.h"

WebServer server(80);

Web::Web()
{
  
}

void Web::main()
{
  // Notify if client has connected to the update server

  
  int current_sta = WiFi.softAPgetStationNum();
  
  if (current_sta < this->num_sta)
  {
    this->num_sta = current_sta;
    Serial.print("Update server: Client disconnected -> ");
    Serial.println(this->num_sta);
  }
  else if (current_sta > this->num_sta)
  {
    this->num_sta = current_sta;
    Serial.print("Update server: Client connected -> ");
    Serial.println(this->num_sta);
  }
  
  
  server.handleClient();
  delay(1);
}

// Callback for the embedded jquery.min.js page
void Web::onJavaScript(void) {
    Serial.println("onJavaScript(void)");
    server.setContentLength(jquery_min_js_v3_2_1_gz_len);
    server.sendHeader(F("Content-Encoding"), F("gzip"));
    server.send_P(200, "text/javascript", jquery_min_js_v3_2_1_gz, jquery_min_js_v3_2_1_gz_len);
}

void Web::setupOTAupdate()
{
  uint8_t newMACAddress[] = {0x06, 0x07, 0x0D, 0x09, 0x0E, 0x0D};

  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, 100);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_WHITE);
  #endif

  Serial.println(wifi_scan_obj.freeRAM());
  #ifdef HAS_SCREEN
    display_obj.tft.print(text_table3[0]);  
  #endif
  Serial.println("Configuring update server...");

  #ifdef HAS_SCREEN
    display_obj.tft.setTextColor(TFT_YELLOW);
  #endif
  
  // Start WiFi AP
  Serial.println("Initializing WiFi...");
  //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_scan_obj.cfg);
  //esp_wifi_set_storage(WIFI_STORAGE_RAM);
  if (esp_wifi_set_storage(WIFI_STORAGE_FLASH) != ESP_OK)
    Serial.println("Could not set WiFi Storage!");
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  Serial.println(wifi_scan_obj.freeRAM());

  Serial.println("Starting softAP...");
  esp_wifi_set_mac(WIFI_IF_AP, &newMACAddress[0]);
  WiFi.softAP(ssid, password);
  Serial.println("");

  Serial.println(wifi_scan_obj.freeRAM());

  Serial.println("Displaying settings to TFT...");
  #ifdef HAS_SCREEN
    display_obj.tft.print(text_table1[2]);
    display_obj.tft.println(ssid);
    display_obj.tft.print(text_table3[1]);
    display_obj.tft.print(WiFi.softAPIP());
    display_obj.tft.print("\n");
  #endif
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // return javascript jquery
  Serial.println("Setting server behavior...");
  Serial.println(wifi_scan_obj.freeRAM());
  server.on("/jquery.min.js", HTTP_GET, onJavaScript);
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, [this]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, [this]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, [this]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [this]() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_YELLOW);
        display_obj.tft.print(text_table3[2]);
        display_obj.tft.print(upload.filename.c_str());
        display_obj.tft.print("\n");
      #endif
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_CYAN);
        display_obj.tft.fillRect(0, 164, 240, 8, TFT_BLACK);
        display_obj.tft.setCursor(0, 164);
        display_obj.tft.print(text_table3[3]);
        display_obj.tft.print(upload.totalSize);
        display_obj.tft.print("\n");
      #endif
      
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_GREEN);
          display_obj.tft.print(text_table3[4]);
          display_obj.tft.print(upload.totalSize);
          display_obj.tft.print(text_table2[3]);
        #endif
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        delay(1000);
      } else {
        Update.printError(Serial);
      }
    }
  });

  
  Serial.println("Finished setting server behavior");
  Serial.println(wifi_scan_obj.freeRAM());
  Serial.println("Starting server...");
  server.begin();

  #ifdef HAS_SCREEN
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(text_table3[5]);
  #endif
  Serial.println("Completed update server setup");
  Serial.println(wifi_scan_obj.freeRAM());
}

void Web::shutdownServer() {
  Serial.println("Closing Update Server...");
  server.stop();
  WiFi.mode(WIFI_OFF);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_stop();
  esp_wifi_deinit();
  Serial.println(wifi_scan_obj.freeRAM());
}
