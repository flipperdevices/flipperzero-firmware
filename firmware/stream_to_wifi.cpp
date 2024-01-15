#include "stream_to_wifi.h"

const char *ssid = "Cisco";
const char *password = "test123";
bool is_wifi_streaming = false;

void stream_to_wifi() {
  if (!is_wifi_streaming) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP: ");
    Serial.println(IP);
    is_wifi_streaming = true;
  }
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
