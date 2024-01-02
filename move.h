#pragma once

#include "game.h"

typedef uint8_t MovabilityTab[SIZE_Y][SIZE_X];

inline uint8_t coord_from(uint8_t x, uint8_t y) {
    return (y * SIZE_X) + x;
}

inline uint8_t coord_x(uint8_t coord) {
    return coord % SIZE_X;
}

inline uint8_t coord_y(uint8_t coord) {
    return coord / SIZE_X;
}

inline uint8_t movable_dir(MovabilityTab* mv, uint8_t currentMovable) {
    return (currentMovable != MOVABLE_NOT_FOUND) ?
               (*mv)[coord_y(currentMovable)][coord_x(currentMovable)] :
               MOVABLE_NOT;
}

void map_movability(PlayGround* pg, MovabilityTab* mv);

uint8_t find_movable(MovabilityTab* mv);
uint8_t find_movable_rev(MovabilityTab* mv);

void find_movable_left(MovabilityTab* mv, uint8_t* currentMovable);
void find_movable_right(MovabilityTab* mv, uint8_t* currentMovable);
void find_movable_up(MovabilityTab* mv, uint8_t* currentMovable);
void find_movable_down(MovabilityTab* mv, uint8_t* currentMovable);
