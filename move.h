#pragma once

#include "common.h"

#define MOVABLE_NOT 0
#define MOVABLE_LEFT 1
#define MOVABLE_RIGHT 2
#define MOVABLE_BOTH 3

#define MOVABLE_NOT_FOUND 128

typedef uint8_t MovabilityTab[SIZE_Y][SIZE_X];

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

inline uint8_t movable_dir(MovabilityTab* mv, uint8_t current_movable) {
    return (current_movable != MOVABLE_NOT_FOUND) ?
               (*mv)[coord_y(current_movable)][coord_x(current_movable)] :
               MOVABLE_NOT;
}

inline void clear_board(PlayGround* ani) {
    memset(ani, '\0', sizeof(uint8_t) * SIZE_X * SIZE_Y);
}

void map_movability(PlayGround* pg, MovabilityTab* mv);

uint8_t find_movable(MovabilityTab* mv);
uint8_t find_movable_rev(MovabilityTab* mv);

void find_movable_left(MovabilityTab* mv, uint8_t* current_movable);
void find_movable_right(MovabilityTab* mv, uint8_t* current_movable);
void find_movable_up(MovabilityTab* mv, uint8_t* current_movable);
void find_movable_down(MovabilityTab* mv, uint8_t* current_movable);
