#pragma once

#include "common.h"

#define MOVABLE_NOT 0
#define MOVABLE_LEFT 1
#define MOVABLE_RIGTH 2
#define MOVABLE_BOTH 3

#define MOVABLE_NOT_FOUND 128

inline bool is_block(uint8_t tile) {
    return (tile > 0) && (tile != WALL_TILE);
}

inline uint8_t coord_from(uint8_t x, uint8_t y) {
    return (y * SIZE_X) + x;
}

inline uint8_t coord_x(uint8_t coord) {
    return coord % SIZE_X;
}

inline uint8_t coord_y(uint8_t coord) {
    return coord / SIZE_X;
}

void map_movability(PlayGround* pg, PlayGround* mv);

uint8_t find_movable(PlayGround* mv);
uint8_t find_movable_rev(PlayGround* mv);

void find_movable_left(PlayGround* mv, uint8_t* current_movable);
void find_movable_right(PlayGround* mv, uint8_t* current_movable);
void find_movable_up(PlayGround* mv, uint8_t* current_movable);
void find_movable_down(PlayGround* mv, uint8_t* current_movable);