#ifndef INITIALIZE_H
#define INITIALIZE_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_model.h"
#include "pins.h"

void initialize(camera_config_t *config);

#endif
