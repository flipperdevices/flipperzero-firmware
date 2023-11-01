#include <stdint.h>
#include <furi.h>
#include "weapon.h"

static uint8_t enemy_frames[2][ENEMY_UI_SIZE] = {
    {
        // id 0 frame 0
        10, 5, // size
        0,  1, 1, 1, 0, 1, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 1, //
        1,  1, 0, 1, 0, 0, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 0, //
        0,  1, 1, 1, 0, 1, 0, 0, 1, 0, //
    },
    {
        // id 0 frame 1
        10, 5, // size
        0,  1, 1, 1, 0, 1, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 1, //
        1,  1, 0, 1, 0, 1, 0, 1, 0, 0, //
        1,  1, 1, 0, 1, 0, 1, 0, 0, 0, //
        0,  1, 1, 1, 0, 1, 0, 0, 1, 0, //
    }};

EnemyTemplate enemies[2] = {
    // id 0
    {1, 10, {0, 1}},
    // id 1
    {2, 15, {0, 1}}
    //
};

void enemy_try_spawn(GameState* const state) {
    if(state->enemies.spawn_order < ENEMY_PULL) {
        if(state->level.enemySpawDelay[state->enemies.spawn_order] <= (state->level_time / FRAMES)) {
            FURI_LOG_E(
                TAG,
                "spawned %d -- %d -- %d\r\n",
                state->enemies.spawn_order,
                state->level.enemySpawDelay[state->enemies.spawn_order],
                state->level_time);

            state->enemies.spawned[state->enemies.spawn_order].points = 1;
            state->enemies.spawned[state->enemies.spawn_order].life = 1;
            state->enemies.spawned[state->enemies.spawn_order].frame = 0;
            state->enemies.spawned[state->enemies.spawn_order].id = levels->enemySpawType[state->enemies.spawn_order];
            state->enemies.spawned[state->enemies.spawn_order].position.x = SCREEN_WIDTH;
            state->enemies.spawned[state->enemies.spawn_order].position.y = 35;
            state->enemies.spawn_order++;
        }
    }
}

void enemy_update(GameState* const state) {
    for(int i = 0; i < ENEMY_PULL; i++) {
        if(state->enemies.spawned[i].position.x <= -10) {
            state->enemies.spawned[i].life = 0;
        }
        if(state->enemies.spawned[i].life > 0) {
            state->enemies.spawned[i].position.x--;

            if(state->level_time % 2 == 0) {
                state->enemies.spawned[i].frame = (state->enemies.spawned[i].frame + 1) % 2;
            }
        }
    }
}