#pragma once

#include <furi.h>

#define TAG "GameVexed"

#define SIZE_X 10
#define SIZE_Y 8
#define TILE_SIZE 8

#define WALL_TILE 9
#define EMPTY_TILE 0

#define MENU_PAUSED_COUNT 6

// -- move -----------------

#define MOVABLE_NOT 0
#define MOVABLE_LEFT 1
#define MOVABLE_RIGHT 2
#define MOVABLE_BOTH 3

#define MOVABLE_NOT_FOUND 128

// -------------------------

typedef uint8_t PlayGround[SIZE_Y][SIZE_X];

typedef struct {
    uint8_t ofBrick[WALL_TILE];
    FuriString* bricksNonZero;
    uint8_t statsNonZero[WALL_TILE + 1];
} Stats;

inline bool is_block(uint8_t tile) {
    return (tile > 0) && (tile != WALL_TILE);
}