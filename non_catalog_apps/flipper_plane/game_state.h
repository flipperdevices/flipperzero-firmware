#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <furi.h>
#include <stdlib.h>
#include "game_state.h"
#include "paper.h"
#include "map.h"

typedef struct GameState {
    Paper* paper;
    FuriTimer* timer;
    uint32_t last_tick;
    FuriMutex* mutex;
    uint16_t* map;
    int crash_flag;
} GameState;

void game_state_init(GameState* const game_state);

void game_state_reinit(GameState* const game_state);

void check_collision(GameState* const game_state);

#endif