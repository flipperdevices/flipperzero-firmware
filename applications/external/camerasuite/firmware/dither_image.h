#ifndef DITHER_IMAGE_H
#define DITHER_IMAGE_H

#include <esp_camera.h>

#include "camera_model.h"

/** Dither the image using the selected algorithm. */
void dither_image(camera_fb_t* frame_buffer);

#endif
