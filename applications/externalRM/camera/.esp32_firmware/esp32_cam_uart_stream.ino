#include "esp_camera.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


void setup() {
  Serial.begin(230400);

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
  config.pixel_format = PIXFORMAT_GRAYSCALE;

  // We don't need a big frame
  config.frame_size = FRAMESIZE_QQVGA;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Setting high contrast to make easier to dither
  sensor_t * s = esp_camera_sensor_get();
  s->set_contrast(s, 2);
}

bool stop_stream = false;
bool disable_dithering = false;
bool invert = false;

void loop() {

  // Reading serial
  if (Serial.available() > 0) {
    char r = Serial.read();
    sensor_t * s = esp_camera_sensor_get();
    
    switch(r) {
      case 'S':
        stop_stream = false;
        break;
      case 's':
        stop_stream = true;
        break;
      case 'D':
        disable_dithering = false;
        break;
      case 'd':
        disable_dithering = true;
        break;
      case 'C':
        s->set_contrast(s, s->status.contrast + 1);
        break;
      case 'c':
        s->set_contrast(s, s->status.contrast - 1);
        break;
      case 'B':
        s->set_contrast(s, s->status.brightness + 1);
        break;
      case 'b':
        s->set_contrast(s, s->status.brightness - 1);
        break;

      // Toggle cases
      case 'M': // Toggle Mirror
        s->set_hmirror(s, !s->status.hmirror);
        break;
      case '>':
        disable_dithering = !disable_dithering;
        break;
      case '<':
        invert = !invert;
      default:
        break;
    }
  }

  if (stop_stream){
    return;
  }

  camera_fb_t* fb = esp_camera_fb_get();

  if (!fb) {
    return;
  }
  
  //Length: 19200
  //Width: 160
  //Height: 120
  //Format: 2
  //Target: 128x64

  if (!disable_dithering) {
    DitherImage(fb);
  }

  uint8_t flipper_y = 0;
  for(uint8_t y = 28; y < 92; ++y) {
    Serial.print("Y:");
    Serial.print((char)flipper_y);

    size_t true_y = y * fb->width;
    for (uint8_t x = 16; x < 144; x+=8){
      char c = 0;
      for(uint8_t j = 0; j < 8; ++j){
        if (IsDarkBit(fb->buf[true_y + x + (7-j)])){
          c |= 1 << j;
        }
      }
      Serial.print(c);
    }

    ++flipper_y;
    Serial.flush();
  }

  esp_camera_fb_return(fb);
  fb = NULL;
  delay(50);
}

bool IsDarkBit(uint8_t bit){
  bool result = bit < 128;

  if (invert){
    result = !result;
  }

  return result;
}

void DitherImage(camera_fb_t* fb) {
  for(uint8_t y = 0; y < fb->height; ++y){
      for (uint8_t x = 0; x < fb->width; ++x){
        size_t current = (y*fb->width) + x;
        uint8_t oldpixel = fb->buf[current];
        uint8_t newpixel = oldpixel >= 128 ? 255 : 0;
        fb->buf[current] = newpixel;
        uint8_t quant_error = oldpixel - newpixel;
        fb->buf[(y*fb->width) + x + 1] = fb->buf[(y*fb->width) + x + 1] + quant_error * 7 / 16;
        fb->buf[(y+1*fb->width) + x-1] = fb->buf[(y+1*fb->width) + x-1] + quant_error * 3 / 16;
        fb->buf[(y + 1*fb->width) + x] = fb->buf[(y + 1*fb->width) + x] + quant_error * 5 / 16;
        fb->buf[(y+1*fb->width) + x+1] = fb->buf[(y+1*fb->width) + x+1] + quant_error * 1 / 16;
      }
    }  
}