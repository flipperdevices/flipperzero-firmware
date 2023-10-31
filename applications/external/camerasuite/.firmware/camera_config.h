#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#include <esp_camera.h>

#include "pins.h"

/** The camera configuration model. */
extern camera_config_t camera_config;

/** Initialize the camera configuration. */
void initialize_camera_config();

#endif
