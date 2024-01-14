#ifndef STREAM_TO_WIFI_H
#define STREAM_TO_WIFI_H

#include <FS.h>
#include <esp_camera.h>

#include "camera.h"
#include "camera_model.h"
#include "dither_image.h"

/** Start the WiFi camera stream. */
void stream_to_wifi();

#endif
