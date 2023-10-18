
//  https://codepen.io/brundolf/pen/beagbQ
//  https://codepen.io/turnwrighthere/pen/GRoqmdK?editors=1100
//  https://codepen.io/collinsworth/pen/LVMRwQ
//  https://manzdev.github.io/twitch-gameboy-css/
//  https://gist.github.com/ShawnHymel/3d124f79aee95fb274876f491a91dcf4
//  https://manzdev.github.io/twitch-gameboy-css/

#include "GameboyServer.h"
#include "Gameboy.h"
#include "configs.h"


void processByte(byte data);
void storeData(byte *image_data);

byte clock_count = 0x00;
byte current_byte = 0x00;
uint32_t packet_count = 0x00;
uint32_t packet_length = 0x00;
byte current_packet_type = 0x00;
bool printed = false;
byte inquiry_count = 0x00;
byte image_data[11520] = {};
uint32_t img_index = 0;
byte last_image_data[11520] = {};
uint32_t last_img_index = 0;
String imageData = "";

unsigned long lastByteReceived = 0;
unsigned long blinkClockHit = 0;
bool blinkCycle = false;
bool successPrinted = false;
bool webConnected = false;
bool initializedAttach = false;


void gbClockHit() {
  if(webConnected) {
    if (digitalRead(GB_MOSI) == HIGH) {
      current_byte |= 0x01;
    }

    if (packet_count == (packet_length - 3)) {
      if (clock_count == 7) {
        digitalWrite(GB_MISO, HIGH);
      }
    }
    if (packet_count == (packet_length - 2)) {
      if (clock_count == 0 || clock_count == 7) {
        digitalWrite(GB_MISO, LOW);
      } else if (clock_count == 6) {
        digitalWrite(GB_MISO, HIGH);
      }
    }

    if (clock_count == 0) {
      // Blink while receiving data
      lastByteReceived = millis();
    }

    if (clock_count == 7) {
      processByte(current_byte);
      clock_count = 0;
      current_byte = 0x00;
    } else {

      successPrinted = false;
      current_byte = current_byte << 1;
      clock_count++;
    }
  }
}

void processByte(byte data) {
  if (packet_count == 2) { //command type
    current_packet_type = data;
    switch (data) {
      case 0x04:
      packet_length = 0x28A; // 650 bytes
      break;

      case 0x02:
      packet_length = 0x0E; // 14 bytes
      break;

      default:
      packet_length = 0x0A; // 10 bytes
      break;
    }
  }

  // Handles that special empty body data packet
  if ((current_packet_type == 0x04) && (packet_count == 4) && (data == 0x00)) {
    packet_length = 0x0A;
  }

  if ((current_packet_type == 0x04) && (packet_count >= 6) && (packet_count <= packet_length - 5)) {
    image_data[img_index++] = data;
    last_img_index = img_index;
  }

  if (current_packet_type == 0x02) {
    printed = true;
  }

  if (printed && (packet_count == 2) && (data == 0x0F)) {
    inquiry_count++;
  }

  if (packet_count == (packet_length - 1)) {
    packet_count = 0x00;
    if (inquiry_count == 4) {
      // storeData(image_data);
      successPrinted = true;
    // 
    }
  } else {
    packet_count++;
  }
}
void resetImageData() {
  for (int i = 0; i < sizeof(image_data); i++) {
    image_data[i] = 0; // Rellenar con valores vacíos (0 en este caso)
  }
}
void resetValues() {
  resetImageData();
  clock_count = 0x00;
  current_byte = 0x00;
  packet_count = 0x00;
  packet_length = 0x00;
  current_packet_type = 0x00;
  printed = false;
  inquiry_count = 0x00;
  img_index = 0x00;

  lastByteReceived = 0;
}

void storeData(byte *image_data) {
  // detachInterrupt(GB_SCLK);

  // unsigned long perf = millis();
  // char fileName[31];
  // sprintf(fileName, "/d/%05d.bin", freeFileIndex);


  // // Send the data in groups of 16 hex values per line via WebSocket
  // char hex_line[50]; // Buffer for the hex line
  // size_t data_sent = 0; // Counter for data sent

  // for (size_t i = 0; i < img_index; i += 16) {
  //   size_t remaining_data = img_index - i;
  //   size_t bytes_to_send = (remaining_data >= 16) ? 16 : remaining_data;

  //   // Build the hex line
  //   char* hex_pos = hex_line;
  //   for (size_t j = 0; j < bytes_to_send; j++) {
  //     snprintf(hex_pos, 4, "%02X ", image_data[i + j]);
  //     hex_pos += 3;
  //   }

  //   // Send the hex line via WebSocket
  //   Serial.println(hex_line);
  //   // webSocket.broadcastTXT(hex_line);
  // }
  // File file = FS.open(fileName, "w");

  // if (!file) {
  //   Serial.println("file creation failed");
  // }

  // file.write("GB-BIN01", 8);
  // file.write(image_data, img_index);
  // file.close();

  // Insert Newline Periodically
  // Serial.println(imageData);
  
    

  // perf = millis() - perf;
  // Serial.printf("File /d/%05d.bin written in %lums\n", freeFileIndex, perf);

  // freeFileIndex++;

  // if (freeFileIndex <= MAX_IMAGES) {
  //   resetValues();
  //   attachInterrupt(GB_SCLK, gbClockHit, RISING);
  // } else {
  //   Serial.println("no more space on printer\nrebooting...");
  //   // ESP.restart();
    // }

  delay(3000);
  resetValues();
  // attachInterrupt(GB_SCLK, gbClockHit, RISING);
}

void espprinter_setup() {
  // Setup ports
  pinMode(GB_MISO, OUTPUT);
  pinMode(GB_MOSI, INPUT);
  pinMode(GB_SCLK, INPUT);
  resetValues();
  // Setup Clock Interrupt
  attachInterrupt(GB_SCLK, gbClockHit, RISING);
}

void espprinter_loop() {
  if (lastByteReceived != 0 && lastByteReceived + 500 < millis()) {
    resetValues();
  }
}
void onResetRequest(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/json");
  detachInterrupt(GB_SCLK);
  resetValues();
  // Setup Clock Interrupt
  attachInterrupt(GB_SCLK, gbClockHit, RISING);
  response->print("{\"success\": true }");
  request->send(response);
}
void onImageRequest(AsyncWebServerRequest *request) {
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
void gb_camera_setup() {
  _index_html = gameboy_camera_index_html_gz;
  _index_html_len = gameboy_camera_index_html_gz_len;
  WiFi.mode(WIFI_AP);
  gbStartAP("[MALVEKE] Flipper GB Cam", "12345678");

  _server.on("/image", HTTP_GET, onImageRequest);
  _server.on("/reset", HTTP_GET, onResetRequest);
  // respond to GET requests on URL /heap
  _server.on("/assets/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", assets_main_js_gz, assets_main_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  
  _server.on("/assets/Input.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", assets_Input_js_gz, assets_Input_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/assets/Select.js", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", assets_Select_js_gz, assets_Select_js_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.on("/assets/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", assets_index_css_gz, assets_index_css_gz_len);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
  });
  _server.addHandler(new GameboyCaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  _server.begin();
}

void gb_camera_loop() {
  _dnsServer.processNextRequest();
  int numClients = WiFi.softAPgetStationNum();
  webConnected = numClients > 0;
  if(webConnected) {
    if(!initializedAttach) {
      espprinter_setup();
      initializedAttach = true;
    } else {
      espprinter_loop();
    }
  }
}
