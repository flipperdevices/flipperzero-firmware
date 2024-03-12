#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#include <esp_camera.h>

#include "camera_model.h"
#include "pins.h"

/** The camera configuration model. */
extern camera_config_t camera_config;

/** Set the camera configuration defaults. */
void set_camera_config_defaults();

#endif
