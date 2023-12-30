#include "move.h"

void map_movability(PlayGround* pg, PlayGround* mv) {
    uint8_t tile, x, y, movable;

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            movable = MOVABLE_NOT;
            tile = (*pg)[y][x];
            if(is_block(tile)) {
                if(x > 0 && ((*pg)[y][x - 1] == EMPTY_TILE)) movable += MOVABLE_LEFT;
                if((x < (SIZE_X - 1)) && ((*pg)[y][x + 1] == EMPTY_TILE)) movable += MOVABLE_RIGTH;
            }
            (*mv)[y][x] = movable;
        }
    }
}

uint8_t find_movable(PlayGround* mv) {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if((*mv)[y][x] != MOVABLE_NOT) return coord_from(x, y);
        }
    }

    return MOVABLE_NOT_FOUND;
}

uint8_t find_movable_rev(PlayGround* mv) {
    uint8_t x, y;
    for(y = SIZE_Y - 1; y > 0; y--) {
        for(x = SIZE_X - 1; x > 0; x--) {
            if((*mv)[y][x] != MOVABLE_NOT) return coord_from(x, y);
        }
    }

    return MOVABLE_NOT_FOUND;
}

void find_movable_left(PlayGround* mv, uint8_t* current_movable) {
    const uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) :
                                                                 SIZE_X / 2;
    const uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t px = sx;

    // to the left, same line
    while(px > 0) {
        px--;
        if((*mv)[sy][px] != MOVABLE_NOT) {
            *current_movable = coord_from(px, sy);
            return;
        }
    }

    uint8_t x, y;
    for(y = sy - 1; y > 0; y--) {
        for(x = SIZE_X - 1; x > 0; x--) {
            if((*mv)[y][x] != MOVABLE_NOT) {
                *current_movable = coord_from(x, y);
                return;
            }
        }
    }

    uint8_t last = find_movable_rev(mv);
    if(last != MOVABLE_NOT_FOUND) {
        *current_movable = last;
    }
}

void find_movable_right(PlayGround* mv, uint8_t* current_movable) {
    const uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) :
                                                                 SIZE_X / 2;
    const uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t px = sx;
    uint8_t py = sy;

    // search right
    while(px < SIZE_X - 1) {
        px++;
        if((*mv)[sy][px] != MOVABLE_NOT) {
            *current_movable = coord_from(px, sy);
            return;
        }
    }

    uint8_t x, y;
    for(y = py + 1; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if((*mv)[y][x] != MOVABLE_NOT) {
                *current_movable = coord_from(x, y);
                return;
            }
        }
    }

    uint8_t first = find_movable(mv);
    if(first != MOVABLE_NOT_FOUND) {
        *current_movable = first;
    }
}

void find_movable_down(PlayGround* mv, uint8_t* current_movable) {
    uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) : SIZE_X / 2;
    uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t py = sy;
    uint8_t delta, px_l, px_r;

    while(py < SIZE_Y - 1) {
        py++;
        if((*mv)[py][sx] != MOVABLE_NOT) {
            *current_movable = coord_from(sx, py);
            return;
        }
    }

    py = sy;
    while(py < SIZE_Y - 1) {
        py++;
        delta = 0;
        while(delta < SIZE_X) {
            delta++;
            px_r = MIN(sx + delta, SIZE_X - 1);
            if((*mv)[py][px_r] != MOVABLE_NOT) {
                *current_movable = coord_from(px_r, py);
                return;
            }
            px_l = MAX(sx - delta, 0);
            if((*mv)[py][px_l] != MOVABLE_NOT) {
                *current_movable = coord_from(px_l, py);
                return;
            }
        }
    }
}

void find_movable_up(PlayGround* mv, uint8_t* current_movable) {
    uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) : SIZE_X / 2;
    uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t py = sy;
    uint8_t delta, px_l, px_r;

    while(py > 0) {
        py--;
        if((*mv)[py][sx] != MOVABLE_NOT) {
            *current_movable = coord_from(sx, py);
            return;
        }
    }

    py = sy;
    while(py > 0) {
        py--;
        delta = 0;
        while(delta < SIZE_X) {
            delta++;
            px_r = MIN(sx + delta, SIZE_X - 1);
            if((*mv)[py][px_r] != MOVABLE_NOT) {
                *current_movable = coord_from(px_r, py);
                return;
            }
            px_l = MAX(sx - delta, 0);
            if((*mv)[py][px_l] != MOVABLE_NOT) {
                *current_movable = coord_from(px_l, py);
                return;
            }
        }
    }
}
