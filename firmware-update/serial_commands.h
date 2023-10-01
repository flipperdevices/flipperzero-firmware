#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <esp_camera.h>
#include <FS.h>

#include "globals.h"

/** Handle the serial input commands coming from the Flipper Zero. */
void serial_commands(CameraModel *model);

#endif
