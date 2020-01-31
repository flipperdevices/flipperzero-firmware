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

void Web::setupOTAupdate()
{
  Serial.println("Configuring update server...");
  
  // Start WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("");
  
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
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();

  Serial.println("Completed update server setup");
}
