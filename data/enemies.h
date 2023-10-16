#include <stdint.h>
#include <furi.h>
#include "weapon.h"

static uint8_t enemies[2][52] = {
    {
        // first ship
        10, 5, // size
        0,  1, 1, 1, 0, 1, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 1, //
        1,  1, 0, 1, 0, 0, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 0, //
        0,  1, 1, 1, 0, 1, 0, 0, 1, 0, //
    },
    {
        // second ship
        10, 5, // size
        0,  1, 1, 1, 0, 1, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 1, //
        1,  1, 0, 1, 0, 0, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 0, //
        0,  1, 1, 1, 0, 1, 0, 0, 1, 0, //
    }};

void enemy_try_spawn(GameState* const state) {
    if(state->enemies.current_spawned < ENEMY_PULL) {
        if(state->level.enemySpawDelay[state->enemies.current_spawned + 1] >= (state->level_time / FRAMES)) {
            FURI_LOG_E(
                TAG,
                "spawned %d -- %d -- %d\r\n",
                state->enemies.current_spawned + 1,
                state->level.enemySpawDelay[state->enemies.current_spawned + 1],
                state->level_time);

            state->enemies.spawned[state->enemies.current_spawned].points = 1;
            state->enemies.spawned[state->enemies.current_spawned].life = 1;
            state->enemies.spawned[state->enemies.current_spawned].id = levels->enemySpawType[state->enemies.current_spawned];
            state->enemies.spawned[state->enemies.current_spawned].position.x = 40 + state->enemies.current_spawned * 5;
            state->enemies.spawned[state->enemies.current_spawned].position.y = 35;
            state->enemies.current_spawned++;
        }
    }
}

void enemy_update(GameState* const state) {
    for(int i = 0; i < ENEMY_PULL; i++) {
        if(state->enemies.spawned[i].life > 0) {
            state->enemies.spawned[i].position.x--;
            //state->enemies.spawned[i].position.y++;
        }
    }
}
