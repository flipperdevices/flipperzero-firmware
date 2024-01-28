#include "stream_to_wifi.h"

const char *ssid = "ESP";
const char *password = "test123";
bool is_wifi_streaming = false;

void stream_to_wifi() {
  if (!is_wifi_streaming) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");

    start_server();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.softAPIP());
    Serial.println("' to connect");

    Serial.flush();
    
    is_wifi_streaming = true;
  }
}

// Todo
void start_server() {
  // server.on("/", HTTP_GET, []() {
  //   if (!camera_model.isStreamToWiFiEnabled) {
  //     start_wifi_stream();
  //   }
  //   server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  //   server.sendHeader("Pragma", "no-cache");
  //   server.sendHeader("Expires", "-1");
  //   server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  //   server.send(200, "text/html", "");
  //   server.sendContent("<html><head></head><body>");
  //   server.sendContent("<img src='stream' style='width: 100%; height: auto;' />");
  //   server.sendContent("</body></html>");
  // });
  // server.on("/stream", HTTP_GET, []() {
  //   if (!camera_model.isStreamToWiFiEnabled) {
  //     start_wifi_stream();
  //   }
  //   String boundary = "ESP32CAM";
  //   String header = "--" + boundary + "\r\nContent-Type: image/jpeg\r\nContent-Length: ";
  //   String jpeg = dither_image();
  //   String response = header + jpeg.length() + "\r\n\r\n" + jpeg + "\r\n";
  //   server.send(200, "multipart/x-mixed-replace; boundary=" + boundary, response);
  // });
  // server.begin();
}

void start_wifi_stream() {
  turn_flash_on();
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
