#include "scientist.h"
#include "game_sprites.h"

#include <jetpack_joyride_icons.h>
#include <gui/gui.h>

void scientist_tick(SCIENTIST* const scientists) {
    for(int i = 0; i < SCIENTISTS_MAX; i++) {
        if(scientists[i].point.x > 0) {
            scientists[i].point.x -= scientists[i].velocity_x; // move based on velocity_x
            if(scientists[i].point.x < -16) { // if the scientist is out of screen
                scientists[i].point.x =
                    0; // set scientist x coordinate to 0 to mark it as "inactive"
            }
        }
    }
}

void spawn_random_scientist(SCIENTIST* const scientists) {
    // Check for an available slot for a new scientist
    for(int i = 0; i < SCIENTISTS_MAX; ++i) {
        if(scientists[i].point.x <= 0 &&
           (rand() % 1000) < 10) { // Spawn rate is less frequent than coins
            scientists[i].state = ScientistStateAlive;
            scientists[i].point.x = 127;
            scientists[i].point.y = 49;
            scientists[i].velocity_x =
                (rand() % (SCIENTIST_VELOCITY_MAX - SCIENTIST_VELOCITY_MIN + 1)) +
                SCIENTIST_VELOCITY_MIN; // random velocity between SCIENTIST_VELOCITY_MIN and SCIENTIST_VELOCITY_MAX
            break;
        }
    }
}

void draw_scientists(const SCIENTIST* scientists, Canvas* const canvas, const GameSprites* sprites) {
    for(int i = 0; i < SCIENTISTS_MAX; ++i) {
        if(scientists[i].point.x > 0) {
            if(scientists[i].state == ScientistStateAlive) {
                canvas_draw_icon_animation(
                    canvas, scientists[i].point.x, scientists[i].point.y, sprites->scientist);
            } else {
                canvas_draw_icon(
                    canvas, scientists[i].point.x, scientists[i].point.y, &I_dead_scientist);
            }
        }
    }
}