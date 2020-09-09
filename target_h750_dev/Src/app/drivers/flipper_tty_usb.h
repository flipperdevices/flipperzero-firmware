#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "../core/furi.h"

bool initTTYUSBDriver(void);
void handle_usb_uart_read(uint8_t* data, uint32_t *size);

#ifdef __cplusplus
}
#endif