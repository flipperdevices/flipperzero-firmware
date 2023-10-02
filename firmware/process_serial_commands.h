#ifndef PROCESS_SERIAL_COMMANDS_H
#define PROCESS_SERIAL_COMMANDS_H

#include <esp_camera.h>

#include "camera.h"
#include "camera_model.h"
#include "pins.h"

/** Handle the serial input commands coming from the Flipper Zero. */
void process_serial_commands();

#endif
