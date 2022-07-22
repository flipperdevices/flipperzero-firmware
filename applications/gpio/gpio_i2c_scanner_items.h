#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <furi_hal_i2c.h>



typedef struct {
    uint8_t items;
    uint8_t responding_address[256];
} I2CScannerState;

void gpio_i2c_scanner_run_once(I2CScannerState* st);
