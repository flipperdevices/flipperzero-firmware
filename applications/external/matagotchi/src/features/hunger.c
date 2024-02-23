#include <stdio.h>
#include <stdint.h>

#include "hunger.h"
#include "../settings_management.h"
#include "../constants.h"
#include "../random_generator.h"

static void init(struct GameState* game_state, uint32_t current_timestamp) {
    game_state->persistent.hu = MAX_HU;
    game_state->persistent.last_recorded_hu_update = current_timestamp;
}

static void check(
    const struct GameState* game_state,
    uint32_t current_timestamp,
    struct GameEvents* game_events) {
    uint32_t last_timestamp = game_state->persistent.last_recorded_hu_update;
    uint32_t nb_events = (current_timestamp - last_timestamp) / LOSE_HU_FREQUENCY;

    FURI_LOG_D(
        LOG_TAG,
        "check_hu(): current_timestamp=%lu; last_timestamp=%lu; nb_events=%lu",
        current_timestamp,
        last_timestamp,
        nb_events);

    // If some events are extracted, the timestamp will be updated
    // even though there are no HP to add.
    game_events->hu_timestamp = (nb_events) ? current_timestamp : last_timestamp;

    while(nb_events-- > 0) {
        if(toss_a_coin(LOSE_HU_PROBABILITY)) {
            game_events->hu -= random_uniform(LOSE_HU_MIN, LOSE_HU_MAX);
        }
    }

    if(game_events->hu) {
        FURI_LOG_I(LOG_TAG, "Lost %ld HU!", -(game_events->hu));
    }
}

static bool apply(struct GameState* game_state, struct GameEvents game_events) {
    int32_t hu = game_events.hu;

    if(game_events.hu_timestamp == 0) {
        // HU events not generated
        return false;
    }

    if(hu < 0) {
        uint32_t lost_hu = (uint32_t)-hu;
        // Lost some HU
        if(game_state->persistent.hu > lost_hu) {
            // There are still HU left
            game_state->persistent.hu -= lost_hu;
        } else {
            // Started to starve
            game_state->persistent.hu = 0;
            FURI_LOG_I(LOG_TAG, "The pet is hungry!");
            play_starvation(game_state);
            vibrate_long(game_state);
        }
    } else if(hu > 0) {
        // Ate some food
        if(game_state->persistent.hu + hu > MAX_HU) {
            // Gained more than max HU
            game_state->persistent.hu = MAX_HU;
        } else {
            game_state->persistent.hu += hu;
        }
    }

    game_state->persistent.last_recorded_hu_update = game_events.hu_timestamp;
    if(hu != 0) {
        FURI_LOG_D(LOG_TAG, "apply_hu(): new total is %lu HU", game_state->persistent.hu);
        return true;
    }
    return false;
}

static int get_text(const struct GameState* game_state, char* str, size_t size) {
    return snprintf(str, size, "HU: %lu/%d\n", game_state->persistent.hu, MAX_HU);
}

struct Feat init_feat_hunger() {
    struct Feat feat;
    feat.init = init;
    feat.check = check;
    feat.apply = apply;
    feat.get_text = get_text;
    return feat;
}
