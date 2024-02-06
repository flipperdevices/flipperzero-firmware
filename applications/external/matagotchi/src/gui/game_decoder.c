#include <furi.h>

#include "game_decoder.h"
#include "matagotchi_icons.h"

/* Index all images in all life stages */
static const Icon* life_stages[LIFE_STAGES_NUM][6] = {
    // EGG
    {&I_egg_00_60x60, &I_egg_01_60x60},
    // BABY
    {&I_baby_00_60x60,
     &I_baby_01_60x60,
     &I_baby_02_60x60,
     &I_baby_03_60x60,
     &I_baby_04_60x60,
     &I_baby_05_60x60},
    // CHILD
    {&I_child_00_60x60, &I_child_01_60x60},
    // TEEN
    {&I_teen_00_60x60, &I_teen_01_60x60},
    // ADULT
    {&I_adult_00_60x60, &I_adult_01_60x60},
    // DEAD
    {&I_dead_00_60x60, &I_dead_01_60x60}};

/* How many images are present in previous struct */
static const uint8_t stage_frames[LIFE_STAGES_NUM] = {
    2, // EGG
    6, // BABY
    2, // CHILD
    2, // TEEN
    2, // ADULT
    2}; // DEAD

const Icon* decode_image(const struct GameState* game_state) {
    enum LifeStage stage = game_state->persistent.stage;
    uint32_t next_index = game_state->next_animation_index;

    return life_stages[stage][next_index % stage_frames[stage]];
}
