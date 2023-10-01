#ifndef SAVE_PICTURE_H
#define SAVE_PICTURE_H

#include <esp_camera.h>
#include <SD_MMC.h>

#include "globals.h"

/**
 * Save the current picture to the onboard SD card.
 * @todo - Future feature.
 */
void save_picture();

#endif
