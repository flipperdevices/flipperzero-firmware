#include "feature_management.h"
#include "constants.h"
#include "random_generator.h"

void init_xp(struct GameState *game_state, uint32_t current_timestamp) {
    game_state->persistent.xp = 0;
    game_state->persistent.last_recorded_xp_update = current_timestamp;
}

void check_xp(const struct GameState *game_state, uint32_t current_timestamp, struct GameEvents *game_events) {
    uint32_t last_timestamp = game_state->persistent.last_recorded_xp_update;
    uint32_t nb_events = (current_timestamp - last_timestamp) / NEW_XP_FREQUENCY;

    // If some events are extracted, the timestamp will be updated
    // even though there are no XP to add.
    game_events->xp_timestamp = (nb_events) ? current_timestamp : last_timestamp;

    while(nb_events-- > 0) {
        if (toss_a_coin(NEW_XP_PROBABILITY)) {
            game_events->xp++;
        }
    }
}

bool apply_xp(struct GameState *game_state, struct GameEvents game_events) {
    bool state_updated = false;

    while(game_events.xp > 0 && game_state->persistent.stage != DEAD) {
        state_updated = true;
        uint32_t max_xp_this_stage = MAX_XP_PER_STAGE[game_state->persistent.stage];

        if (game_state->persistent.xp + game_events.xp >= max_xp_this_stage) {
            game_events.xp -= max_xp_this_stage - game_state->persistent.xp;
            game_state->persistent.xp = 0;
            game_state->persistent.stage++;
        } else {
            game_state->persistent.xp += game_events.xp;
            game_events.xp = 0;
        }
    }
    game_state->persistent.last_recorded_xp_update = game_events.xp_timestamp;
    return state_updated;
}
