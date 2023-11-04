#ifndef INITIALIZE_CAMERA_H
#define INITIALIZE_CAMERA_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_config.h"
#include "camera_model.h"

/** Initialize the camera. */
void initialize_camera();

/** Reset the camera to the default settings. */
void reset_camera();

/** Turn the flash off. */
void turn_flash_off();

/** Turn the flash on. */
void turn_flash_on();

/** If the flash state ever gets out of sync with the camera model, fix it. */
void handle_flash_state();

/** Add contrast to the camera. */
void add_contrast();

/** Lower contrast on the camera. */
void lower_contrast();

/** Add brightness to the camera. */
void add_brightness();

/** Lower brightness on the camera. */
void lower_brightness();

/** Set the horizontal mirroring state of the camera. */
void set_hmirror(bool is_hmirror_enabled);

#endif
