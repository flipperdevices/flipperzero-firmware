#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <esp_camera.h>

#include "camera.h"
#include "camera_config.h"
#include "camera_model.h"
#include "process_image_to_serial.h"
#include "process_serial_input.h"

void setup();

void loop();

void streamToSerial();

void streamToWiFi();

#endif
