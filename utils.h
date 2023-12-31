#pragma once

#include <furi.h>

#include "game.h"

inline uint8_t cap_x(uint8_t coord) {
    return MIN(MAX(0, coord), (SIZE_X - 1));
}

inline uint8_t cap_y(uint8_t coord) {
    return MIN(MAX(0, coord), (SIZE_Y - 1));
}