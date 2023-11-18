#include "GameBoyCameraServer.h"
#include "soc/rtc_wdt.h"

static GameBoyCameraServer* gameBoyCameraServerInstance;

static void gbClockHitWrapper() {
    gameBoyCameraServerInstance->gbClockHit();
}

static void onResetRequestWrapper(AsyncWebServerRequest *request) {
    gameBoyCameraServerInstance->onResetRequest(request);
}


static void onImageRequestWrapper(AsyncWebServerRequest *request) {
    gameBoyCameraServerInstance->onImageRequest(request);
}

GameBoyCameraServer::GameBoyCameraServer()
{
    this->runGameBoyCameraServer = false;
    gameBoyCameraServerInstance = this;
    
}
void GameBoyCameraServer::begin()
{
    this->setup();
}

void GameBoyCameraServer::processByte(uint8_t data) {
  if (this->packet_count == 2) { //command type
    this->current_packet_type = data;
    switch (data) {
      case 0x04:
      this->packet_length = 0x28A; // 650 bytes
      break;

      case 0x02:
      this->packet_length = 0x0E; // 14 bytes
      break;

      default:
      this->packet_length = 0x0A; // 10 bytes
      break;
    }
  }

  // Handles that special empty body data packet
  if ((this->current_packet_type == 0x04) && (this->packet_count == 4) && (data == 0x00)) {
    this->packet_length = 0x0A;
  }

  if ((this->current_packet_type == 0x04) && (this->packet_count >= 6) && (this->packet_count <= this->packet_length - 5)) {
    this->image_data[this->img_index++] = data;
    this->last_img_index = this->img_index;
  }

  if (this->current_packet_type == 0x02) {
    this->printed = true;
  }

  if (printed && (this->packet_count == 2) && (data == 0x0F)) {
    this->inquiry_count++;
  }

  if (this->packet_count == (this->packet_length - 1)) {
    this->packet_count = 0x00;
    if (this->inquiry_count == 4) {
      // this->storeData(image_data);
      this->successPrinted = true;
    // 
    }
  } else {
    this->packet_count++;
  }
}
void GameBoyCameraServer::gbClockHit() {
  if(this->webConnected) {
    if (digitalRead(GB_MOSI) == HIGH) {
      this->current_byte |= 0x01;
    }

    if (this->packet_count == (this->packet_length - 3)) {
      if (this->clock_count == 7) {
        digitalWrite(GB_MISO, HIGH);
      }
    }
    if (this->packet_count == (this->packet_length - 2)) {
      if (this->clock_count == 0 || this->clock_count == 7) {
        digitalWrite(GB_MISO, LOW);
      } else if (this->clock_count == 6) {
        digitalWrite(GB_MISO, HIGH);
      }
    }

    if (this->clock_count == 0) {
      // Blink while receiving data
      this->lastByteReceived = millis();
    }

    if (this->clock_count == 7) {
      this->processByte(this->current_byte);
      this->clock_count = 0;
      this->current_byte = 0x00;
    } else {
      this->successPrinted = false;
      this->current_byte = this->current_byte << 1;
      this->clock_count++;
    }
  }
}

void GameBoyCameraServer::resetImageData() {
  for (int i = 0; i < sizeof(this->image_data); i++) {
    this->image_data[i] = 0; // Rellenar con valores vacíos (0 en este caso)
  }
}
void GameBoyCameraServer::resetValues() {
  this->resetImageData();
  this->clock_count = 0x00;
  this->current_byte = 0x00;
  this->packet_count = 0x00;
  this->packet_length = 0x00;
  this->current_packet_type = 0x00;
  this->printed = false;
  this->inquiry_count = 0x00;
  this->img_index = 0x00;

  this->lastByteReceived = 0;
}

void GameBoyCameraServer::storeData(byte *image_data) {
  // detachInterrupt(GB_SCLK);
  // if (freeFileIndex <= MAX_IMAGES) {
  //   resetValues();
  //   attachInterrupt(GB_SCLK, gbClockHit, RISING);
  // } else {
  //   Serial.println("no more space on printer\nrebooting...");
  //   // ESP.restart();
    // }

  delay(3000);
  this->resetValues();
  // attachInterrupt(GB_SCLK, gbClockHit, RISING);
}
void GameBoyCameraServer::espprinter_setup() {
  // Setup ports
  pinMode(GB_MISO, OUTPUT);
  pinMode(GB_MOSI, INPUT);
  pinMode(GB_SCLK, INPUT);
  this->resetValues();
  // Setup Clock Interrupt
  attachInterrupt(GB_SCLK, gbClockHitWrapper, RISING);
}
void GameBoyCameraServer::espprinter_loop() {
  if (this->lastByteReceived != 0 && this->lastByteReceived + 500 < millis()) {
    this->resetValues();
  }
}
void GameBoyCameraServer::onResetRequest(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/json");
  detachInterrupt(GB_SCLK);
  this->resetValues();
  // Setup Clock Interrupt
  attachInterrupt(GB_SCLK, gbClockHitWrapper, RISING);
  response->print("{\"success\": true }");
  request->send(response);
}

void GameBoyCameraServer::onImageRequest(AsyncWebServerRequest *request) {
  int from = 0;
  if (request->hasParam("from")) {
      String fromValue = request->getParam("from")->value();
      from = fromValue.toInt();
  }
  AsyncResponseStream *response = request->beginResponseStream("text/json");
  response->print("{\"printed\": ");
  response->print(printed);
  response->print(", \"packet_length\": ");
  response->print(packet_length);
  response->print(", \"packet_count\": ");
  response->print(packet_count);
  response->print(", \"from\": ");
  response->print(from);
  response->print(", \"last\": ");
  response->print(last_img_index);
  response->print(", \"success\": ");
  response->print(successPrinted);
  response->print(", \"dump\":[");

  for (size_t i = from; i < last_img_index; i++) {
    response->print((uint8_t)image_data[i]);
    if(i < last_img_index -1) {
      response->print(",");
    }
  }

  response->print("]}");
  request->send(response);
}
bool GameBoyCameraServer::isRunning() {
    return this->runGameBoyCameraServer;
}
void GameBoyCameraServer::stop()
{
    transferJSON.clear();
    WiFi.softAPdisconnect (true);
    _server.end(); 
    this->runGameBoyCameraServer = false;
    ESP.restart();  /*ESP restart function*/
}
void GameBoyCameraServer::start()
{
    transferJSON.clear();
    _index_html = index_html_gz;
    _index_html_len = index_html_gz_len;
    WiFi.mode(WIFI_AP);
    gbStartAP("[MALVEKE] Flipper GB Cam", "12345678");


    _server.on("/favicon.ico.gz", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon_ico_gz, favicon_ico_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    _server.on("/malveke_pattern.png.gz", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse_P(200, "image/png", malveke_pattern_png_gz, malveke_pattern_png_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    _server.on("/image", HTTP_GET, onImageRequestWrapper);
    _server.on("/reset", HTTP_GET, onResetRequestWrapper);
    _server.addHandler(new GameboyCaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    _server.begin();

    this->runGameBoyCameraServer = true;
}
void GameBoyCameraServer::setup()
{
    // disableCore0WDT();
    this->runGameBoyCameraServer = false;
}
void GameBoyCameraServer::main()
{
    //  Loop: 
    if(this->runGameBoyCameraServer) {
        _dnsServer.processNextRequest();
        int numClients = WiFi.softAPgetStationNum();
        webConnected = numClients > 0;
        if(webConnected) {
            if(!this->initializedAttach) {
                this->espprinter_setup();
                this->initializedAttach = true;
            } else {
                this->espprinter_loop();
            }
        }
    }
}