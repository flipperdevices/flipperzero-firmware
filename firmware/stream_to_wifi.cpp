#include "stream_to_wifi.h"

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "ESP";
const char *password = "test123";
bool is_wifi_streaming = false;

AsyncWebServer server(80);

#define MAX_HTML_SIZE 20000

char index_html[MAX_HTML_SIZE] = "TEST";

void stream_to_wifi() {
  if (!is_wifi_streaming) {
    // Connect to WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");

    // Start the web server
    start_server();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.softAPIP());
    Serial.println("' to connect");

    Serial.flush();

    is_wifi_streaming = true;
  }
}

void start_server() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!camera_model.isStreamToWiFiEnabled) {
      start_wifi_stream();
    }
    request->send_P(200, "text/html", index_html);
  });

  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!camera_model.isStreamToWiFiEnabled) {
      start_wifi_stream();
    }

    String boundary = "ESP32CAM";
    String header = "--" + boundary + "\r\nContent-Type: image/jpeg\r\nContent-Length: ";
    String jpeg = dither_image();
    String response = header + jpeg.length() + "\r\n\r\n" + jpeg + "\r\n";
    request->send(200, "multipart/x-mixed-replace; boundary=" + boundary, response);
  });

  // Serve additional resources like images, stylesheets, etc. if needed.
  // server.serveStatic("/img", SPIFFS, "/img");

  server.begin();
}

void start_wifi_stream() {
  turn_flash_on(); // Physical test indicator that we're streaming.
  camera_model.isStreamToSerialEnabled = false;
  set_camera_config_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_model_defaults(CAMERA_FUNCTION_WIFI);
  set_camera_defaults(CAMERA_FUNCTION_WIFI);
  // @todo - Dynamically set ssid and password via prompts.
  camera_model.isStreamToWiFiEnabled = true;
  turn_flash_off();
}

void stop_wifi_stream() {
  if (is_wifi_streaming) {
    WiFi.softAPdisconnect(true);
    is_wifi_streaming = false;
    camera_model.isStreamToWiFiEnabled = false;
  }
}
