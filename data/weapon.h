#include <stdint.h>
#include "constants.h"

#define BULLET_X 12
#define BULLET_OFFSET 3
#define BULLET_NEXT BULLET_X + 5

void weapon_try_fire_bullet(GameState* const state) {
    int8_t slot = -1;
    for(int i = 0; i < BULLET_PULL; i++) {
        if(state->player.bullets[i].x > 0 && state->player.bullets[i].x < BULLET_NEXT) {
            return;
        }

        if(state->player.bullets[i].x == 0) {
            slot = i;
        }
    }

    if(slot >= 0) {
        state->player.bullets[slot].x = BULLET_X;
        state->player.bullets[slot].y = state->player.position.y + BULLET_OFFSET;
    }
}

void weapon_update(GameState* const state) {
    for(int i = 0; i < BULLET_PULL; i++) {
        if(state->player.bullets[i].x > SCREEN_WIDTH) {
            state->player.bullets[i].x = 0;
            state->player.bullets[i].y = 0;
        }
        if(state->player.bullets[i].x >= BULLET_X) {
            state->player.bullets[i].x++;
        }
    }
}

void weapon_check_colisions(GameState* const state) {
    for(int i = 0; i < BULLET_PULL; i++) {
        if(state->player.bullets[i].x > SCREEN_WIDTH || state->player.bullets[i].x == 0) {
            continue; // bullet is not active
        }

        for(int e = 0; e < ENEMY_PULL; e++) {
            if(state->enemies.spawned[e].position.x == 0 || state->enemies.spawned[e].life == 0) {
                continue; // enemy is dead or out of screen
            }

            if(state->player.bullets[i].x >= state->enemies.spawned[e].position.x &&
               state->player.bullets[i].x <= state->enemies.spawned[e].position.x + 8 &&
               state->player.bullets[i].y >= state->enemies.spawned[e].position.y &&
               state->player.bullets[i].y <= state->enemies.spawned[e].position.y + 5) {
                state->player.bullets[i].x = 0;
                state->player.bullets[i].y = 0;
                state->enemies.spawned[e].life--;

                if(state->enemies.spawned[e].life <= 0) {
                    state->enemies.spawned[e].position.x = 0;
                    state->enemies.spawned[e].position.y = 0;
                    state->player.score += 10;

                    // add destroy animation
                    for(int d = 0; d < ENEMY_PULL; d++) {
                        if(state->destroy[d].frame == 0) {
                            state->destroy[d].frame = 1;
                            state->destroy[d].pos.x = state->enemies.spawned[e].position.x;
                            state->destroy[d].pos.y = state->enemies.spawned[e].position.y;
                            break;
                        }
                    }
                }
            }
        }
    }
}