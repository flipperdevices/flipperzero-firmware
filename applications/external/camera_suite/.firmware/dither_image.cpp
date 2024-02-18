#include "dither_image.h"

void dither_image(camera_fb_t *frame_buffer) {
  for (uint8_t y = 0; y < frame_buffer->height; ++y) {
    for (uint8_t x = 0; x < frame_buffer->width; ++x) {
      size_t current = (y * frame_buffer->width) + x;
      uint8_t oldpixel = frame_buffer->buf[current];
      uint8_t newpixel = oldpixel >= 128 ? 255 : 0;
      frame_buffer->buf[current] = newpixel;
      int8_t quant_error = oldpixel - newpixel;

      // Apply error diffusion based on the selected algorithm
      switch (camera_model.ditherAlgorithm) {
      case JARVIS_JUDICE_NINKE:
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] +=
            quant_error * 7 / 48;
        frame_buffer->buf[(y * frame_buffer->width) + x + 2] +=
            quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 2] +=
            quant_error * 3 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] +=
            quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] +=
            quant_error * 7 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] +=
            quant_error * 5 / 48;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 2] +=
            quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 2] +=
            quant_error * 1 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 1] +=
            quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x] +=
            quant_error * 5 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 1] +=
            quant_error * 3 / 48;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 2] +=
            quant_error * 1 / 48;
        break;
      case STUCKI:
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] +=
            quant_error * 8 / 42;
        frame_buffer->buf[(y * frame_buffer->width) + x + 2] +=
            quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 2] +=
            quant_error * 2 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] +=
            quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] +=
            quant_error * 8 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] +=
            quant_error * 4 / 42;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 2] +=
            quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 2] +=
            quant_error * 1 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x - 1] +=
            quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x] +=
            quant_error * 4 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 1] +=
            quant_error * 2 / 42;
        frame_buffer->buf[(y + 2) * frame_buffer->width + x + 2] +=
            quant_error * 1 / 42;
        break;
      case FLOYD_STEINBERG:
      default:
        // Default to Floyd-Steinberg dithering if an invalid
        // algorithm is selected
        frame_buffer->buf[(y * frame_buffer->width) + x + 1] +=
            quant_error * 7 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x - 1] +=
            quant_error * 3 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x] +=
            quant_error * 5 / 16;
        frame_buffer->buf[(y + 1) * frame_buffer->width + x + 1] +=
            quant_error * 1 / 16;
        break;
      }
    }
  }
}
