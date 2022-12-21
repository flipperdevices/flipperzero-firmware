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