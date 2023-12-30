#pragma once

#include <furi.h>

#define TAG "GameVexed"
#define SIZE_X 10
#define SIZE_Y 8
#define tileSize 8

#define WALL_TILE 9
#define EMPTY_TILE 0

typedef uint8_t PlayGround[SIZE_Y][SIZE_X];

inline uint8_t cap_x(uint8_t coord) {
    return MIN(MAX(0, coord), (SIZE_X - 1));
}

inline uint8_t cap_y(uint8_t coord) {
    return MIN(MAX(0, coord), (SIZE_Y - 1));
}