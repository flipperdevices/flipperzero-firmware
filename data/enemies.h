#include <stdint.h>
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
        if(state->level.enemySpawDelay[game_state->enemies.current_spawned + 1] >= (game_state->level_time / FRAMES)) {
            FURI_LOG_E(
                TAG,
                "spawned %d -- %d -- %d\r\n",
                game_state->enemies.current_spawned + 1,
                game_state->level.enemySpawDelay[game_state->enemies.current_spawned + 1],
                game_state->level_time);

            game_state->enemies.spawned[game_state->enemies.current_spawned].points = 1;
            game_state->enemies.spawned[game_state->enemies.current_spawned].life = 1;
            game_state->enemies.spawned[game_state->enemies.current_spawned].id = levels->enemySpawType[game_state->enemies.current_spawned];
            game_state->enemies.spawned[game_state->enemies.current_spawned].position.x = 40 + game_state->enemies.current_spawned * 5;
            game_state->enemies.spawned[game_state->enemies.current_spawned].position.y = 35;
            game_state->enemies.current_spawned++;
        }
    }
}

void enemy_update(GameState* const state) {
    for(int i = 0; i < ENEMY_PULL; i++) {
        if(state->enemies.spawned[i].life > 0) {
            state->enemies.spawned[i].points.x--;
            //state->enemies.spawned[i].points.y++;
        }
    }
}
