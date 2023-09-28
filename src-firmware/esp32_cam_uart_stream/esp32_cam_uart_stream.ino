#include "esp_camera.h"

// Define Pin numbers used by the camera
#define FLASH_GPIO_NUM    4
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define SIOC_GPIO_NUM     27
#define SIOD_GPIO_NUM     26
#define XCLK_GPIO_NUM      0
#define VSYNC_GPIO_NUM    25

#define Y2_GPIO_NUM        5
#define Y3_GPIO_NUM       18
#define Y4_GPIO_NUM       19
#define Y5_GPIO_NUM       21
#define Y6_GPIO_NUM       36
#define Y7_GPIO_NUM       39
#define Y8_GPIO_NUM       34
#define Y9_GPIO_NUM       35

// Structure to hold the camera configuration parameters
camera_config_t config;

// Function prototypes
void handleSerialInput();
void initializeCamera();
void processImage(camera_fb_t* fb);
void ditherImage(camera_fb_t* fb);

// Enumeration to represent the available dithering algorithms
enum DitheringAlgorithm {
  FLOYD_STEINBERG,
  JARVIS_JUDICE_NINKE,
  STUCKI
};

// Variables to hold state and configurations
DitheringAlgorithm ditherAlgorithm = FLOYD_STEINBERG; // Holds the currently selected dithering algorithm
bool disableDithering = false; // Flag to enable or disable dithering
bool invert = false; // Flag to invert pixel colors
bool isFlashOn = false; // Flag to represent the flash state
bool rotated = false; // Flag to represent whether the image is rotated
bool stopStream = false; // Flag to stop or start the stream

void setup() {
  // Start serial communication at 230400 baud rate
  Serial.begin(230400);
  initializeCamera();
}

void loop() {
  if (!stopStream) {
    // Capture and process the frame buffer if streaming is enabled
    camera_fb_t* fb = esp_camera_fb_get(); // Get the frame buffer from the camera
    if (fb) {
      processImage(fb);
      // Return the frame buffer back to the camera driver
      esp_camera_fb_return(fb);
    }
    delay(50); // Delay for 50ms between each frame
  }

  handleSerialInput(); // Handle any available serial input commands
}

// Function to handle the serial input commands and perform the associated actions
void handleSerialInput() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    sensor_t* cameraSensor = esp_camera_sensor_get();

    switch (input) {
      case '>': // Toggle dithering
        disableDithering = !disableDithering;
        break;
      case '<': // Toggle invert
        invert = !invert;
        break;
      case 'B': // Add brightness
        cameraSensor->set_contrast(cameraSensor, cameraSensor->status.brightness + 1);
        break;
      case 'b': // Remove brightness
        cameraSensor->set_contrast(cameraSensor, cameraSensor->status.brightness - 1);
        break;
      case 'C': // Add contrast
        cameraSensor->set_contrast(cameraSensor, cameraSensor->status.contrast + 1);
        break;
      case 'c': // Remove contrast
        cameraSensor->set_contrast(cameraSensor, cameraSensor->status.contrast - 1);
        break;
      case 'P': // Picture sequence.
        if (!isFlashOn) {
          isFlashOn = true;
          pinMode(FLASH_GPIO_NUM, OUTPUT);
          // Turn on torch.
          digitalWrite(FLASH_GPIO_NUM, HIGH); 
          delay(2000);
          // Turn off torch.
          digitalWrite(FLASH_GPIO_NUM, LOW); 
          delay(50);
          isFlashOn = false;
        }
        break;
      case 'M': // Toggle Mirror
        cameraSensor->set_hmirror(cameraSensor, !cameraSensor->status.hmirror);
        break;
      case 'S': // Start stream
        stopStream = false;
        break;
      case 's': // Stop stream
        stopStream = true;
        break;
      case '0': // Use Floyd Steinberg dithering.
        ditherAlgorithm = FLOYD_STEINBERG;
        break;
      case '1': // Use Jarvis Judice dithering.
        ditherAlgorithm = JARVIS_JUDICE_NINKE;
        break;
      case '2': // Use Stucki dithering.
        ditherAlgorithm = STUCKI;
        break;
      default:
        // Do nothing.
        break;
    }
  }
}

void initializeCamera() {
  // Set camera configurations
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
  config.frame_size = FRAMESIZE_QQVGA;
  config.fb_count = 1;

  if (isFlashOn) {
    pinMode(FLASH_GPIO_NUM, OUTPUT);
    // Turn off torch.
    digitalWrite(FLASH_GPIO_NUM, LOW); 
    isFlashOn = false;
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Set initial contrast.
  sensor_t* s = esp_camera_sensor_get();
  s->set_contrast(s, 0);

  // Set rotation
  s->set_vflip(s, true);  // Vertical flip
  s->set_hmirror(s, true);  // Horizontal mirror
}

void processImage(camera_fb_t* frameBuffer) {
  // If dithering is not disabled, perform dithering on the image. Dithering is the 
  // process of approximating the look of a high-resolution grayscale image in a 
  // lower resolution by binary values (black & white), thereby representing
  // different shades of gray.
  if (!disableDithering) {
    ditherImage(frameBuffer); // Invokes the dithering process on the frame buffer
  }

  uint8_t flipper_y = 0;

  // Iterating over specific rows of the frame buffer.
  for (uint8_t y = 28; y < 92; ++y) {
    Serial.print("Y:"); // Print "Y:" for every new row.
    Serial.write(flipper_y); // Send the row identifier as a byte.
    
    // Calculate the actual y index in the frame buffer 1D array by multiplying the 
    // y value with the width of the frame buffer. This gives the starting index 
    // of the row in the 1D array.
    size_t true_y = y * frameBuffer->width;
    
    // Iterating over specific columns of each row in the frame buffer.
    for (uint8_t x = 16; x < 144; x += 8) { // step by 8 as we're packing 8 pixels per byte
        uint8_t packed_pixels = 0;
        // Packing 8 pixel values into one byte.
        for(uint8_t bit = 0; bit < 8; ++bit) {
             // Check the invert flag and pack the pixels accordingly.
            if(invert) {
                // If invert is true, consider pixel as 1 if it's more than 127.
                if(frameBuffer->buf[true_y + x + bit] > 127) {
                    packed_pixels |= (1 << (7 - bit));
                }
            } else {
                // If invert is false, consider pixel as 1 if it's less than 127.
                if(frameBuffer->buf[true_y + x + bit] < 127) {
                    packed_pixels |= (1 << (7 - bit));
                }
            }
        }
        Serial.write(packed_pixels); // Sending packed pixel byte.
    }

    ++flipper_y; // Move to the next row.
    Serial.flush(); // Ensure all data in the Serial buffer is sent before moving to the next iteration.
  }
}

void ditherImage(camera_fb_t* fb) {
  for (uint8_t y = 0; y < fb->height; ++y) {
    for (uint8_t x = 0; x < fb->width; ++x) {
      size_t current = (y * fb->width) + x;
      uint8_t oldpixel = fb->buf[current];
      uint8_t newpixel = oldpixel >= 128 ? 255 : 0;
      fb->buf[current] = newpixel;
      int8_t quant_error = oldpixel - newpixel;

      // Apply error diffusion based on the selected algorithm
      switch (ditherAlgorithm) {
        case JARVIS_JUDICE_NINKE:
          fb->buf[(y * fb->width) + x + 1] += quant_error * 7 / 48;
          fb->buf[(y * fb->width) + x + 2] += quant_error * 5 / 48;
          fb->buf[(y + 1) * fb->width + x - 2] += quant_error * 3 / 48;
          fb->buf[(y + 1) * fb->width + x - 1] += quant_error * 5 / 48;
          fb->buf[(y + 1) * fb->width + x] += quant_error * 7 / 48;
          fb->buf[(y + 1) * fb->width + x + 1] += quant_error * 5 / 48;
          fb->buf[(y + 1) * fb->width + x + 2] += quant_error * 3 / 48;
          fb->buf[(y + 2) * fb->width + x - 2] += quant_error * 1 / 48;
          fb->buf[(y + 2) * fb->width + x - 1] += quant_error * 3 / 48;
          fb->buf[(y + 2) * fb->width + x] += quant_error * 5 / 48;
          fb->buf[(y + 2) * fb->width + x + 1] += quant_error * 3 / 48;
          fb->buf[(y + 2) * fb->width + x + 2] += quant_error * 1 / 48;
          break;
        case STUCKI:
          fb->buf[(y * fb->width) + x + 1] += quant_error * 8 / 42;
          fb->buf[(y * fb->width) + x + 2] += quant_error * 4 / 42;
          fb->buf[(y + 1) * fb->width + x - 2] += quant_error * 2 / 42;
          fb->buf[(y + 1) * fb->width + x - 1] += quant_error * 4 / 42;
          fb->buf[(y + 1) * fb->width + x] += quant_error * 8 / 42;
          fb->buf[(y + 1) * fb->width + x + 1] += quant_error * 4 / 42;
          fb->buf[(y + 1) * fb->width + x + 2] += quant_error * 2 / 42;
          fb->buf[(y + 2) * fb->width + x - 2] += quant_error * 1 / 42;
          fb->buf[(y + 2) * fb->width + x - 1] += quant_error * 2 / 42;
          fb->buf[(y + 2) * fb->width + x] += quant_error * 4 / 42;
          fb->buf[(y + 2) * fb->width + x + 1] += quant_error * 2 / 42;
          fb->buf[(y + 2) * fb->width + x + 2] += quant_error * 1 / 42;
          break;
        case FLOYD_STEINBERG:
        default:
          // Default to Floyd-Steinberg dithering if an invalid algorithm is selected
          fb->buf[(y * fb->width) + x + 1] += quant_error * 7 / 16;
          fb->buf[(y + 1) * fb->width + x - 1] += quant_error * 3 / 16;
          fb->buf[(y + 1) * fb->width + x] += quant_error * 5 / 16;
          fb->buf[(y + 1) * fb->width + x + 1] += quant_error * 1 / 16;
          break;
      }
    }
  }
}
