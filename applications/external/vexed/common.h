#pragma once

#include <furi.h>

#define TAG "GameVexed"

#define SIZE_X 10
#define SIZE_Y 8
#define TILE_SIZE 8

#define SIZE_X_BG 17 // 128/8 +1
#define SIZE_Y_BG 9 // 64/8  +1

#define WALL_TILE 9
#define EMPTY_TILE 0

#define MENU_PAUSED_COUNT 6
#define MAIN_MENU_COUNT 3

#define PAR_LABEL_SIZE 10

// -- move -----------------

#define MOVABLE_NOT 0
#define MOVABLE_LEFT 1
#define MOVABLE_RIGHT 2
#define MOVABLE_BOTH 3

#define MOVABLE_NOT_FOUND 128

// -------------------------

typedef uint8_t PlayGround[SIZE_Y][SIZE_X];
typedef uint8_t BackGround[SIZE_Y_BG][SIZE_X_BG];

typedef struct {
    uint8_t ofBrick[WALL_TILE];
    FuriString* bricksNonZero;
    uint8_t statsNonZero[WALL_TILE + 1];
} Stats;
