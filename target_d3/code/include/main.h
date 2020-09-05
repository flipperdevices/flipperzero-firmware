#pragma once

#include <device.h>

typedef struct device* SerialDevice;
void app_serial_write(SerialDevice* serial, uint8_t* buffer, uint16_t size, uint32_t timeout);
