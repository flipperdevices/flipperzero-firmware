#ifndef INITIALIZE_CAMERA_H
#define INITIALIZE_CAMERA_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_config.h"
#include "camera_model.h"

/** Initialize the camera. */
void initialize_camera();

/** Turn the flash off. */
void toggle_flash_off();

/** Turn the flash on. */
void toggle_flash_on();

/** If the flash state ever gets out of sync with the camera model, fix it. */
void handle_flash_state();

#endif
