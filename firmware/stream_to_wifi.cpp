#include "stream_to_wifi.h"

// Constants
const char *password = "test123";
const char *ssid = "ESP";
bool is_wifi_streaming = false;
char index_html[MAX_HTML_SIZE] = "TEST";
AsyncWebServer server(80);

void stream_to_wifi() {
  Serial.println("Starting");

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
  start_wifi_stream();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request){
  //   camera_fb_t *fb = esp_camera_fb_get();  // Capture a frame
  //   if (fb) {
  //     request->send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  //     esp_camera_fb_return(fb);  // Release the frame buffer
  //   } else {
  //     request->send(500);  // Internal Server Error
  //   }
  // });

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
    // WiFi.softAPdisconnect(true);
    is_wifi_streaming = false;
    camera_model.isStreamToWiFiEnabled = false;
  }
}
