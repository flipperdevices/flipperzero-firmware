#ifndef DITHERING_H
#define DITHERING_H

#include <esp_camera.h>

#include "globals.h"

void dither_image(camera_fb_t *frame_buffer, CameraModel *model);

#endif
