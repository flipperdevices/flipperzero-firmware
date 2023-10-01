#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_model.h"
#include "pins.h"

/** Handle the serial input commands coming from the Flipper Zero. */
void serial_commands();

#endif
