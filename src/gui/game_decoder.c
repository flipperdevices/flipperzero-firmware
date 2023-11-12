#include <furi.h>

#include "game_decoder.h"
#include "matagotchi_icons.h"

const Icon *decode_image(const struct GameState *game_state) {
    uint32_t next_index = game_state->next_animation_index;

    switch(game_state->persistent.stage) {
        case EGG:
            if(next_index % 2) {
                return &I_egg_01_60x60;
            } else {
                return &I_egg_00_60x60;
            }
        case BABY:
            if(next_index % 6 == 0) {
                return &I_baby_00_60x60;
            } else if(next_index % 6 == 1) {
                return &I_baby_01_60x60;
            } else if(next_index % 6 == 2) {
                return &I_baby_02_60x60;
            } else if(next_index % 6 == 3) {
                return &I_baby_03_60x60;
            } else if(next_index % 6 == 4) {
                return &I_baby_04_60x60;
            } else {
                return &I_baby_05_60x60;
            }
        case CHILD:
            if(next_index % 2) {
                return &I_child_01_60x60;
            } else {
                return &I_child_00_60x60;
            }
        case TEEN:
            if(next_index % 2) {
                return &I_teen_01_60x60;
            } else {
                return &I_teen_00_60x60;
            }
        case ADULT:
            if(next_index % 2) {
                return &I_adult_01_60x60;
            } else {
                return &I_adult_00_60x60;
            }
        case DEAD:
            if(next_index % 2) {
                return &I_dead_01_60x60;
            } else {
                return &I_dead_00_60x60;
            }
        default:
            furi_crash("Unexpected stage in decode_image");
    }
}
