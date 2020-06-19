#include "Web.h"

WebServer server(80);

Web::Web()
{
  
}

void Web::main()
{
  //Serial.println("Running the shits");
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
  
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_WHITE);

  Serial.println(wifi_scan_obj.freeRAM());
  display_obj.tft.print("Configuring update server...\n\n");  
  Serial.println("Configuring update server...");

  display_obj.tft.setTextColor(TFT_YELLOW);
  
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
  esp_wifi_set_mac(ESP_IF_WIFI_AP, &newMACAddress[0]);
  WiFi.softAP(ssid, password);
  Serial.println("");

  Serial.println(wifi_scan_obj.freeRAM());

  Serial.println("Displaying settings to TFT...");
  display_obj.tft.print("SSID: ");
  display_obj.tft.println(ssid);
  display_obj.tft.print("IP address: ");
  display_obj.tft.print(WiFi.softAPIP());
  display_obj.tft.print("\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  /*use mdns for host name resolution*/
  /*
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  */

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
      display_obj.tft.setTextColor(TFT_YELLOW);
      display_obj.tft.print("Update: ");
      display_obj.tft.print(upload.filename.c_str());
      display_obj.tft.print("\n");
      //display_obj.updateBanner(menu_function_obj.current_menu->name);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
      //display_obj.tft.println(upload.totalSize);
      /*
      String display_string = "";
      display_obj.tft.setCursor(0, 164);
      for (int i = 0; i < 40; i++) {
        display_string.concat(" ");
      }
      */
      display_obj.tft.setTextColor(TFT_CYAN);
      display_obj.tft.fillRect(0, 164, 240, 8, TFT_BLACK);
      //delay(1);
      //display_obj.tft.print(display_string);
      display_obj.tft.setCursor(0, 164);
      display_obj.tft.print("Bytes complete: ");
      display_obj.tft.print(upload.totalSize);
      display_obj.tft.print("\n");
      
      //Serial.println(upload.totalSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        display_obj.tft.setTextColor(TFT_GREEN);
        display_obj.tft.print("Update Success: ");
        display_obj.tft.print(upload.totalSize);
        display_obj.tft.print("\nRebooting...\n");
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

  display_obj.tft.setTextColor(TFT_GREEN);
  display_obj.tft.println("\nCompleted update server setup");
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
