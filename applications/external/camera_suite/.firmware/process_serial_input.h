#ifndef PROCESS_SERIAL_INPUT_H
#define PROCESS_SERIAL_INPUT_H

#include <esp_camera.h>

#include "camera.h"
#include "camera_model.h"
#include "pins.h"
#include "stream_to_serial.h"

/** Handle the serial input commands coming from the Flipper Zero. */
void process_serial_input();

#endif
