#include <furi.h>
#include <stdlib.h>
#include "game_state.h"
#include "paper.h"
#include "map.h"

void game_state_init(GameState* const game_state) {
    game_state->last_tick = furi_get_tick();
    game_state->crash_flag = 0;
    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    Paper* paper = malloc(sizeof(Paper));
    paper_init(paper);

    uint16_t* map = malloc(sizeof(uint16_t) * MAP_HEIGHT);
    init_map(map);

    game_state->paper = paper;
    game_state->map = map;
}

void game_state_reinit(GameState* const game_state) {
    game_state->last_tick = furi_get_tick();
    game_state->crash_flag = 0;

    paper_init(game_state->paper);
}

void check_collision(GameState* const game_state) {
    /*
        to make collision detection easier, 
        convert the uint16_t to an array of 
        uint8_t's
    */
    uint8_t currentRow[sizeof(uint16_t) * 8];
    uint16_t mapCopy = game_state->map[(int)game_state->paper->y + 3];
    for(unsigned int j = 0; j < sizeof(uint16_t) * 8; j++) {
        if(mapCopy & 0x8000) {
            currentRow[j] = 1;
        } else {
            currentRow[j] = 0;
        }

        mapCopy <<= 1;
    }

    // TODO: this collision code barely works, needs a refactor
    if(currentRow[(unsigned int)(game_state->paper->x + 0.375)] ||
       currentRow[(unsigned int)(game_state->paper->x + 0.625)]) {
        game_state->crash_flag = 1;
    }
}