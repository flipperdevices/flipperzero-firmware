#include <furi.h> // For FURI_LOG_D

#include "feature_management.h"
#include "constants.h"
#include "random_generator.h"

/* Other functions */

void correct_state(struct GameState* game_state) {
    if(game_state->persistent.stage == DEAD) {
        game_state->persistent.xp = 0;
        game_state->persistent.hu = 0;
        game_state->persistent.hp = 0;
    }
}

void generate_hu(
    struct GameState* game_state,
    uint32_t current_timestamp,
    struct GameEvents* game_events) {
    if(game_state->persistent.stage != DEAD && game_state->persistent.hu < MAX_HU) {
        game_events->hu = random_uniform(MIN_CANDY_HU_RESTORE, MAX_CANDY_HU_RESTORE);
        game_events->hu_timestamp = current_timestamp;
    }
}

void generate_hp(
    struct GameState* game_state,
    uint32_t current_timestamp,
    struct GameEvents* game_events) {
    if(game_state->persistent.stage != DEAD && game_state->persistent.hp < MAX_HP) {
        game_events->hp = random_uniform(MIN_PILL_HP_RESTORE, MAX_PILL_HP_RESTORE);
        game_events->hp_timestamp = current_timestamp;
    }
}
