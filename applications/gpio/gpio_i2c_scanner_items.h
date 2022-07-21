#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef struct {
    uint8_t items;
    uint8_t responding_address[256];
} I2CScannerState;
