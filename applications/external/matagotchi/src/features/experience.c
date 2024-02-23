#include <stdio.h>
#include <stdint.h>

#include "experience.h"
#include "../settings_management.h"
#include "../constants.h"
#include "../random_generator.h"

static void init(struct GameState* game_state, uint32_t current_timestamp) {
    game_state->persistent.xp = 0;
    game_state->persistent.last_recorded_xp_update = current_timestamp;
}

static void check(
    const struct GameState* game_state,
    uint32_t current_timestamp,
    struct GameEvents* game_events) {
    uint32_t last_timestamp = game_state->persistent.last_recorded_xp_update;
    uint32_t nb_events = (current_timestamp - last_timestamp) / NEW_XP_FREQUENCY;

    FURI_LOG_D(
        LOG_TAG,
        "check_xp(): current_timestamp=%lu; last_timestamp=%lu; nb_events=%lu",
        current_timestamp,
        last_timestamp,
        nb_events);

    // If some events are extracted, the timestamp will be updated
    // even though there are no XP to add.
    game_events->xp_timestamp = (nb_events) ? current_timestamp : last_timestamp;

    while(nb_events-- > 0) {
        if(toss_a_coin(NEW_XP_PROBABILITY)) {
            game_events->xp++;
        }
    }

    if(game_events->xp) {
        FURI_LOG_I(LOG_TAG, "Gained %lu new XP!", game_events->xp);
    }
}

static bool apply(struct GameState* game_state, struct GameEvents game_events) {
    bool state_updated = false;

    if(game_events.xp_timestamp == 0) {
        // XP events not generated
        return false;
    }

    while(game_events.xp > 0 && game_state->persistent.stage != DEAD) {
        state_updated = true;
        uint32_t max_xp_this_stage = MAX_XP_PER_STAGE[game_state->persistent.stage];

        if(game_state->persistent.xp + game_events.xp >= max_xp_this_stage) {
            FURI_LOG_D(
                LOG_TAG,
                "apply_xp(): need to apply %lu new XP, can only %lu "
                "in current stage %u (%lu XP already present in current stage)",
                game_events.xp,
                max_xp_this_stage - game_state->persistent.xp,
                game_state->persistent.stage,
                game_state->persistent.xp);

            game_events.xp -= max_xp_this_stage - game_state->persistent.xp;
            game_state->persistent.xp = 0;
            game_state->persistent.stage++;
            FURI_LOG_I(LOG_TAG, "Evoluted to new stage %u!", game_state->persistent.stage);
            play_level_up(game_state);
            vibrate_long(game_state);
        } else {
            game_state->persistent.xp += game_events.xp;
            game_events.xp = 0;
            FURI_LOG_D(LOG_TAG, "apply_xp(): new total is %lu XP", game_state->persistent.xp);
        }
    }
    game_state->persistent.last_recorded_xp_update = game_events.xp_timestamp;
    return state_updated;
}

static int get_text(const struct GameState* game_state, char* str, size_t size) {
    return snprintf(
        str,
        size,
        "XP: %lu/%lu\n",
        game_state->persistent.xp,
        MAX_XP_PER_STAGE[game_state->persistent.stage]);
}

struct Feat init_feat_experience() {
    struct Feat feat;
    feat.init = init;
    feat.check = check;
    feat.apply = apply;
    feat.get_text = get_text;
    return feat;
}
