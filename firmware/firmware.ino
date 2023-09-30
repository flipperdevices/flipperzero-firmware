#include "FS.h"
#include "esp_camera.h"
#include "SD_MMC.h"

// Define Pin numbers used by the camera.
#define FLASH_GPIO_NUM 4
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define SIOC_GPIO_NUM 27
#define SIOD_GPIO_NUM 26
#define XCLK_GPIO_NUM 0
#define VSYNC_GPIO_NUM 25

#define Y2_GPIO_NUM 5
#define Y3_GPIO_NUM 18
#define Y4_GPIO_NUM 19
#define Y5_GPIO_NUM 21
#define Y6_GPIO_NUM 36
#define Y7_GPIO_NUM 39
#define Y8_GPIO_NUM 34
#define Y9_GPIO_NUM 35

// Structure to hold the camera configuration parameters.
camera_config_t config;

// Function prototypes.
void handleSerialInput();
void initializeCamera();
void processImage(camera_fb_t *frame_buffer);
void ditherImage(camera_fb_t *frame_buffer);
void saveJpegPictureToSDCard();

// Enumeration to represent the available dithering algorithms.
enum DitheringAlgorithm : uint8_t
{
  FLOYD_STEINBERG,
  JARVIS_JUDICE_NINKE,
  STUCKI
};

// Holds the currently selected dithering algorithm.
DitheringAlgorithm ditherAlgorithm = FLOYD_STEINBERG;

// Flag to enable or disable dithering.
bool disableDithering = false;

// Flag to invert pixel colors.
bool invert = false;

// Flag to represent the flash state when saving pictures to the Flipper.
bool isFlashEnabled = false;

// Flag to represent whether the image is rotated.
bool rotated = false;

// Flag to stop or start the stream.
bool stopStream = false;

// Flag to store jpeg images to sd card.
bool storeJpeg = false;

void setup()
{
  Serial.begin(230400); // 115200
  initializeCamera();
}

void loop()
{
  if (!stopStream)
  {
    camera_fb_t *frame_buffer = esp_camera_fb_get();
    if (frame_buffer)
    {
      // Process and Send Grayscale image.
      processImage(frame_buffer);
      // Return the frame buffer back to the camera driver.
      esp_camera_fb_return(frame_buffer);
    }
    delay(25);
  }
  // Handle any available serial input commands.
  handleSerialInput();
}

void handleSerialInput()
{
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    sensor_t *cameraSensor = esp_camera_sensor_get();

    switch (input)
    {
    case '>': // Toggle dithering.
      disableDithering = !disableDithering;
      break;
    case '<': // Toggle invert.
      invert = !invert;
      break;
    case 'b': // Remove brightness.
      cameraSensor->set_contrast(
          cameraSensor,
          cameraSensor->status.brightness - 1);
      break;
    case 'B': // Add brightness.
      cameraSensor->set_contrast(
          cameraSensor,
          cameraSensor->status.brightness + 1);
      break;
    case 'c': // Remove contrast.
      cameraSensor->set_contrast(
          cameraSensor,
          cameraSensor->status.contrast - 1);
      break;
    case 'C': // Add contrast.
      cameraSensor->set_contrast(
          cameraSensor,
          cameraSensor->status.contrast + 1);
      break;
    case 'f': // Toggle flash off.
      isFlashEnabled = false;
      break;
    case 'F': // Toggle flash on.
      isFlashEnabled = true;
      break;
    case 'j': // Toggle store jpeg to sd card off.
      storeJpeg = false;
      break;
    case 'J': // Toggle store jpeg to sd card on.
      storeJpeg = true;
      break;
    case 'P': // Picture sequence.
      if (isFlashEnabled)
      {
        // Turn on torch.
        pinMode(FLASH_GPIO_NUM, OUTPUT);
        digitalWrite(FLASH_GPIO_NUM, HIGH);
        // Give some time (500ms) for Flipper to save locally with flash on.
        delay(500);
        // Turn off torch.
        digitalWrite(FLASH_GPIO_NUM, LOW);
      }

      // @todo - Future feature.
      // if (storeJpeg) { saveJpegPictureToSDCard(); }
      break;
    case 'M': // Toggle Mirror.
      cameraSensor->set_hmirror(cameraSensor, !cameraSensor->status.hmirror);
      break;
    case 's': // Stop stream.
      stopStream = true;
      break;
    case 'S': // Start stream.
      stopStream = false;
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

void initializeCamera()
{
  // Set initial camera configurations for grayscale.
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

  // Initialize camera.
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    return;
  }

  // Make sure torch starts as off.
  pinMode(FLASH_GPIO_NUM, OUTPUT);
  digitalWrite(FLASH_GPIO_NUM, LOW);

  // Set initial contrast.
  sensor_t *s = esp_camera_sensor_get();
  s->set_contrast(s, 0);

  // Set rotation.
  s->set_vflip(s, true);   // Vertical flip.
  s->set_hmirror(s, true); // Horizontal mirror.
}

void processImage(camera_fb_t *frame_buffer)
{
  // If dithering is not disabled, perform dithering on the image. Dithering is the
  // process of approximating the look of a high-resolution grayscale image in a
  // lower resolution by binary values (black & white), thereby representing
  // different shades of gray.
  if (!disableDithering)
  {
    ditherImage(frame_buffer); // Invokes the dithering process on the frame buffer.
  }

  uint8_t flipper_y = 0;

  // Iterating over specific rows of the frame buffer.
  for (uint8_t y = 28; y < 92; ++y)
  {
    Serial.print("Y:");      // Print "Y:" for every new row.
    Serial.write(flipper_y); // Send the row identifier as a byte.

    // Calculate the actual y index in the frame buffer 1D array by multiplying the
    // y value with the width of the frame buffer. This gives the starting index of
    // the row in the 1D array.
    size_t true_y = y * frame_buffer->width;

    // Iterating over specific columns of each row in the frame buffer.
    for (uint8_t x = 16; x < 144; x += 8)
    { // step by 8 as we're packing 8 pixels per byte.
      uint8_t packed_pixels = 0;
      // Packing 8 pixel values into one byte.
      for (uint8_t bit = 0; bit < 8; ++bit)
      {
        // Check the invert flag and pack the pixels accordingly.
        if (invert)
        {
          // If invert is true, consider pixel as 1 if it's more than 127.
          if (frame_buffer->buf[true_y + x + bit] > 127)
          {
            packed_pixels |= (1 << (7 - bit));
          }
        }
        else
        {
          // If invert is false, consider pixel as 1 if it's less than 127.
          if (frame_buffer->buf[true_y + x + bit] < 127)
          {
            packed_pixels |= (1 << (7 - bit));
          }
        }
      }
      Serial.write(packed_pixels); // Sending packed pixel byte.
    }

    ++flipper_y;    // Move to the next row.
    Serial.flush(); // Ensure all data in the Serial buffer is sent before moving to the next iteration.
  }
}

void ditherImage(camera_fb_t *frame_buffer)
{
  for (uint8_t y = 0; y < frame_buffer->height; ++y)
  {
    for (uint8_t x = 0; x < frame_buffer->width; ++x)
    {
      size_t current = (y * frame_buffer->width) + x;
      uint8_t oldpixel = frame_buffer->buf[current];
      uint8_t newpixel = oldpixel >= 128 ? 255 : 0;
      frame_buffer->buf[current] = newpixel;
      int8_t quant_error = oldpixel - newpixel;

      // Apply error diffusion based on the selected algorithm
      switch (ditherAlgorithm)
      {
      case JARVIS_JUDICE_NINKE:
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] += quant_error * 7 / 48;
        frame_buffer->buf[(y * frame_buffer->width) + x + 2] += quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 2] += quant_error * 3 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] += quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] += quant_error * 7 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] += quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 2] += quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 2] += quant_error * 1 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 1] += quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x] += quant_error * 5 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 1] += quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 2] += quant_error * 1 / 48;
        break;
      case STUCKI:
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] += quant_error * 8 / 42;
        frame_buffer->buf[(y * frame_buffer->width) + x + 2] += quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 2] += quant_error * 2 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] += quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] += quant_error * 8 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] += quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 2] += quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 2] += quant_error * 1 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 1] += quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x] += quant_error * 4 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 1] += quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 2] += quant_error * 1 / 42;
        break;
      case FLOYD_STEINBERG:
      default:
        // Default to Floyd-Steinberg dithering if an invalid algorithm is selected
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] += quant_error * 7 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] += quant_error * 3 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] += quant_error * 5 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] += quant_error * 1 / 16;
        break;
      }
    }
  }
}

// @todo - Future feature.
void saveJpegPictureToSDCard()
{
  // Get camera sensor.
  sensor_t *s = esp_camera_sensor_get();

  // Check if the sensor is valid.
  if (!s)
  {
    Serial.println("Failed to acquire camera sensor");
    return;
  }

  // Set pixel format to JPEG for saving picture.
  s->set_pixformat(s, PIXFORMAT_JPEG);

  // Set frame size based on available PSRAM.
  if (psramFound())
  {
    s->set_framesize(s, FRAMESIZE_UXGA);
  }
  else
  {
    s->set_framesize(s, FRAMESIZE_SVGA);
  }

  // Get a frame buffer from camera.
  camera_fb_t *frame_buffer = esp_camera_fb_get();
  if (!frame_buffer)
  {
    Serial.println("Camera capture failed");
    return;
  }

  if (!SD_MMC.begin())
  {
    // SD Card Mount Failed.
    Serial.println("SD Card Mount Failed");
    esp_camera_fb_return(frame_buffer);
    return;
  }

  // Generate a unique filename.
  String path = "/picture";
  path += String(millis());
  path += ".jpg";

  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);

  if (!file)
  {
    Serial.println("Failed to open file in writing mode");
  }
  else
  {
    if (file.write(frame_buffer->buf, frame_buffer->len) != frame_buffer->len)
    {
      Serial.println("Failed to write the image to the file");
    }
    file.close(); // Close the file in any case.
  }

  // Update framesize back to the default.
  s->set_framesize(s, FRAMESIZE_QQVGA);

  // Return the frame buffer back to the camera driver.
  esp_camera_fb_return(frame_buffer);
}
