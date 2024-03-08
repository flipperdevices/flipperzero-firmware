#include "stream_to_serial.h"

void stream_to_serial() {
  camera_fb_t *frame_buffer = esp_camera_fb_get();

  // If dithering is not disabled, perform dithering on the image. Dithering
  // is the process of approximating the look of a high-resolution grayscale
  // image in a lower resolution by binary values (black & white), thereby
  // representing different shades of gray.
  if (camera_model.isDitheringEnabled) {
    // Invokes the dithering process on the frame buffer.
    dither_image(frame_buffer);
  }

  uint8_t flipper_y = 0;

  // Iterating over specific rows of the frame buffer.
  for (uint8_t y = 28; y < 92; ++y) {
    Serial.print("Y:");      // Print "Y:" for every new row.
    Serial.write(flipper_y); // Send the row identifier as a byte.

    // Calculate the actual y index in the frame buffer 1D array by
    // multiplying the y value with the width of the frame buffer. This
    // gives the starting index of the row in the 1D array.
    size_t true_y = y * frame_buffer->width;

    // Iterating over specific columns of each row in the frame buffer.
    for (uint8_t x = 16; x < 144;
         x += 8) { // step by 8 as we're packing 8 pixels per byte.
      uint8_t packed_pixels = 0;
      // Packing 8 pixel values into one byte.
      for (uint8_t bit = 0; bit < 8; ++bit) {
        // Check the invert flag and pack the pixels accordingly.
        if (camera_model.isInvertEnabled) {
          // If invert is true, consider pixel as 1 if it's more than
          // 127.
          if (frame_buffer->buf[true_y + x + bit] > 127) {
            packed_pixels |= (1 << (7 - bit));
          }
        } else {
          // If invert is false, consider pixel as 1 if it's less than
          // 127.
          if (frame_buffer->buf[true_y + x + bit] < 127) {
            packed_pixels |= (1 << (7 - bit));
          }
        }
      }
      Serial.write(packed_pixels); // Sending packed pixel byte.
    }
    // Move to the next row.
    ++flipper_y;
    // Ensure all data in the Serial buffer is sent before moving to the
    // next iteration.
    Serial.flush();
  }

  // Return the frame buffer back to the camera driver.
  esp_camera_fb_return(frame_buffer);
}

void start_serial_stream() {
  set_camera_config_defaults();
  set_camera_model_defaults();
  set_camera_defaults();
  camera_model.isStreamToSerialEnabled = true;
}

void stop_serial_stream() {
  camera_model.isStreamToSerialEnabled = false;
}
