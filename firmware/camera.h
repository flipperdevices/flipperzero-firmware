#ifndef CAMERA_H
#define CAMERA_H

#include <FS.h>
#include <esp_camera.h>

#include "camera_config.h"
#include "camera_model.h"

/** Initialize the camera. */
void initialize_camera();

/** Reset the camera to the default settings. */
void set_camera_defaults();

/** Turn the flash off. */
void turn_flash_off();

/** Turn the flash on. */
void turn_flash_on();

/** Add contrast to the camera. */
void add_contrast();

/** Lower contrast of the camera. */
void lower_contrast();

/** Add brightness to the camera. */
void add_brightness();

/** Lower brightness of the camera. */
void lower_brightness();

/** Set dithering on or off for the serial output of the camera. */
void set_dithering(bool is_dithering_enabled);

/** Set the dithering algorithm used for the serial output of the camera. */
void set_dithering_algorithm(DitheringAlgorithm dithering_algorithm);

/** Set the horizontal mirroring state of the camera. */
void set_hmirror(bool is_hmirror_enabled);

/** Set the inversion of the pixel output for the serial output of the camera */
void set_inverted(bool is_inverted_enabled);

#endif
