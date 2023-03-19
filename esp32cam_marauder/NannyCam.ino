#include <WiFi.h>
#include "esp_camera.h"

// Replace with your network credentials
const char* ssid = "Flipper0";
const char* password = "12345678";

// Create an instance of the nannyServer
WiFiServer nannyServer(80);

// Set camera resolution
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void nanny_cam_setup() {
  //Serial.begin(115200);

  // Start access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);


  // Print IP address
  IPAddress IP = WiFi.softAPIP();

  // Start the camera
  startCamera();

  // Start the nannyServer
  nannyServer.begin();
  //Serial.println("Server started");

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PW: ");
  Serial.println(password);
  Serial.print("IP: ");
  Serial.println(IP);

}

void nanny_cam_loop() {
  //Serial.print(".");
  // Wait for a client to connect
  WiFiClient client = nannyServer.available();
  if (client) {
    Serial.println("New client connected");
    handleStreamRequest();
  }
}

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size: 640x480
  config.frame_size = FRAMESIZE_VGA;

  // Two buffers for streaming video
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Set the focus to infinity
  /*sensor_t* s = esp_camera_sensor_get();
  s->set_focus(s, 0);*/
}

void handleStreamRequest() {
  WiFiClient client = nannyServer.available();

  // Send HTTP headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println("");

  // Start streaming JPEG frames
  while (client.connected()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      break;
    }

    // Send frame header
    client.println("--frame");
    client.println("Content-Type: image/jpeg");
    client.print("Content-Length: ");
    client.println(fb->len);
    client.println("");

    // Send frame data
    client.write(fb->buf, fb->len);
    client.println("");

    // Release the buffer
    esp_camera_fb_return(fb);
  }

  // Disconnect the client
  client.stop();
  Serial.println("Client disconnected");
}
