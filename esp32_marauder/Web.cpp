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
  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_WHITE);

  display_obj.tft.print("Configuring update server...\n\n");  
  Serial.println("Configuring update server...");

  display_obj.tft.setTextColor(TFT_YELLOW);
  
  // Start WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("");

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
  server.begin();

  display_obj.tft.setTextColor(TFT_GREEN);
  display_obj.tft.println("\nCompleted update server setup");
  Serial.println("Completed update server setup");
}
