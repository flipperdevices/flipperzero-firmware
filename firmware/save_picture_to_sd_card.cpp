#include "save_picture_to_sd_card.h"

void save_picture_to_sd_card() {
  sensor_t *cam = esp_camera_sensor_get();

  // Check if the sensor is valid.
  if (!cam) {
    Serial.println("Failed to acquire camera sensor");
    return;
  }

  // Set pixel format to JPEG for saving picture.
  cam->set_pixformat(cam, PIXFORMAT_JPEG);

  // Set frame size based on available PSRAM.
  if (psramFound()) {
    cam->set_framesize(cam, FRAMESIZE_UXGA);
  } else {
    cam->set_framesize(cam, FRAMESIZE_SVGA);
  }

  // Get a frame buffer from camera.
  camera_fb_t *frame_buffer = esp_camera_fb_get();
  if (!frame_buffer) {
    // Camera capture failed
    return;
  }

  if (!SD_MMC.begin()) {
    // SD Card Mount Failed.
    esp_camera_fb_return(frame_buffer);
    return;
  }

  // Generate a unique filename.
  String path = "/picture";
  path += String(millis());
  path += ".jpg";

  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);

  if (!file) {
    // Failed to open file in writing mode
  } else {
    if (file.write(frame_buffer->buf, frame_buffer->len) != frame_buffer->len) {
      // Failed to write the image to the file
    }
    file.close(); // Close the file in any case.
  }

  // Update framesize back to the default.
  cam->set_framesize(cam, FRAMESIZE_QQVGA);

  // Return the frame buffer back to the camera driver.
  esp_camera_fb_return(frame_buffer);
}
