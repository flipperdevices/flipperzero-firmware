#ifndef STREAM_TO_SERIAL_H
#define STREAM_TO_SERIAL_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_model.h"
#include "dither_image.h"

/** Process and send grayscale images back to the Flipper Zero. */
void stream_to_serial();

#endif
