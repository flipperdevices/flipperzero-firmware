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

void find_movable_left(PlayGround* mv, uint8_t* current_movable) {
    uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) : SIZE_X / 2;
    uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t px = sx;

    while(px > 0) {
        px--;
        if((*mv)[sy][px] != MOVABLE_NOT) {
            *current_movable = coord_from(px, sy);
            return;
        }
    }

    while(sy > 0) {
        sy--;
        px = sx;

        while(px > 0) {
            if((*mv)[sy][px] != MOVABLE_NOT) {
                *current_movable = coord_from(px, sy);
                return;
            }
            px--;
        }

        px = sx + 1;

        while(px < SIZE_X - 1) {
            if((*mv)[sy][px] != MOVABLE_NOT) {
                *current_movable = coord_from(px, sy);
                return;
            }
            px++;
        }
    }
}

void find_movable_right(PlayGround* mv, uint8_t* current_movable) {
    uint8_t sx = (*current_movable != MOVABLE_NOT_FOUND) ? coord_x(*current_movable) : SIZE_X / 2;
    uint8_t sy = (*current_movable != MOVABLE_NOT_FOUND) ? coord_y(*current_movable) : 0;
    uint8_t px = sx;

    while(px < SIZE_X - 1) {
        px++;
        if((*mv)[sy][px] != MOVABLE_NOT) {
            *current_movable = coord_from(px, sy);
            return;
        }
    }

    while(sy < SIZE_Y - 1) {
        sy++;
        px = sx;

        while(px < SIZE_X - 1) {
            if((*mv)[sy][px] != MOVABLE_NOT) {
                *current_movable = coord_from(px, sy);
                return;
            }
            px++;
        }

        px = sx - 1;

        while(px > 0) {
            if((*mv)[sy][px] != MOVABLE_NOT) {
                *current_movable = coord_from(px, sy);
                return;
            }
            px--;
        }
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
