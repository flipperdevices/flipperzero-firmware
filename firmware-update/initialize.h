#ifndef INITIALIZE_H
#define INITIALIZE_H

#include <esp_camera.h>
#include <FS.h>

#include "globals.h"

camera_config_t config;

void initialize(CameraModel *model);

#endif
