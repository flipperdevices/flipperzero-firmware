#include <stdlib.h>
#include <stdbool.h>

#include <jetpack_joyride_icons.h>
#include <gui/gui.h>

#include "states.h"
#include "game_sprites.h"
#include "missile.h"
#include "barry.h"

void missile_tick(MISSILE* const missiles, BARRY* const barry, State* const state) {
    // Move missiles towards the player
    for(int i = 0; i < MISSILES_MAX; i++) {
        if(missile_colides(&missiles[i], barry)) {
            (*state) = GameStateGameOver;
            UNUSED(state);
        }
        if(missiles[i].point.x > 0) {
            missiles[i].point.x -= 2; // move left by 2 units
            if(missiles[i].point.x < -16) { // if the coin is out of screen
                missiles[i].point.x = 0; // set coin x coordinate to 0 to mark it as "inactive"
            }
        }
    }
}

bool missile_colides(MISSILE* const missile, BARRY* const barry) {
    return !(
        barry->point.x > missile->point.x + 26 || // Barry is to the right of the missile
        barry->point.x + 11 < missile->point.x || // Barry is to the left of the missile
        barry->point.y > missile->point.y + 12 || // Barry is below the missile
        barry->point.y + 15 < missile->point.y); // Barry is above the missile
}

void spawn_random_missile(MISSILE* const missiles) {
    // Check for an available slot for a new coin
    for(int i = 0; i < MISSILES_MAX; ++i) {
        if(missiles[i].point.x <= 0) {
            missiles[i].point.x = 127;
            missiles[i].point.y = rand() % 64;
            break;
        }
    }
}

void draw_missiles(const MISSILE* missiles, Canvas* const canvas, const GameSprites* sprites) {
    for(int i = 0; i < MISSILES_MAX; ++i) {
        if(missiles[i].point.x > 0) {
            canvas_draw_icon_animation(
                canvas, missiles[i].point.x, missiles[i].point.y, sprites->missile);
        }
    }
}