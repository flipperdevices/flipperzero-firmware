#include <stdio.h>
#include <stdint.h>

#include "health.h"
#include "../settings_management.h"
#include "../constants.h"
#include "../random_generator.h"

static void init(struct GameState* game_state, uint32_t current_timestamp) {
    game_state->persistent.hp = MAX_HP;
    game_state->persistent.last_recorded_hp_update = current_timestamp;
}

static void check(
    const struct GameState* game_state,
    uint32_t current_timestamp,
    struct GameEvents* game_events) {
    uint32_t last_timestamp = game_state->persistent.last_recorded_hp_update;
    uint32_t nb_events = (current_timestamp - last_timestamp) / CHECK_HP_FREQUENCY;

    FURI_LOG_D(
        LOG_TAG,
        "check_hp(): current_timestamp=%lu; last_timestamp=%lu; nb_events=%lu",
        current_timestamp,
        last_timestamp,
        nb_events);

    // If some events are extracted, the timestamp will be updated
    // even though there are no HP to add.
    game_events->hp_timestamp = (nb_events) ? current_timestamp : last_timestamp;

    while(nb_events-- > 0) {
        // If the pet is hungry or if he got sick
        if(!game_state->persistent.hu || toss_a_coin(LOSE_HP_PROBABILITY)) {
            if(!game_state->persistent.hu) {
                FURI_LOG_I(LOG_TAG, "The pet is losing HP for starvation!");
            } else {
                FURI_LOG_I(LOG_TAG, "The pet is losing HP for an illness!");
                play_ambulance(game_state);
                vibrate_long(game_state);
            }
            game_events->hp -= random_uniform(LOSE_HP_MIN, LOSE_HP_MAX);
        }
    }

    if(game_events->hp) {
        FURI_LOG_I(LOG_TAG, "Lost %ld HP!", -(game_events->hp));
    }
}

static bool apply(struct GameState* game_state, struct GameEvents game_events) {
    int32_t hp = game_events.hp;

    if(game_events.hp_timestamp == 0) {
        // HP events not generated
        return false;
    }

    if(hp < 0) {
        uint32_t lost_hp = (uint32_t)-hp;
        // Lost some HP
        if(game_state->persistent.hp > lost_hp) {
            // There are still HP left
            game_state->persistent.hp -= lost_hp;
        } else {
            // Dead
            game_state->persistent.hp = 0;
            game_state->persistent.stage = DEAD;
            FURI_LOG_I(LOG_TAG, "The pet is dead!");
        }
    } else if(hp > 0) {
        // Gained some HP
        if(game_state->persistent.hp + hp > MAX_HP) {
            // Gained more than max HP
            game_state->persistent.hp = MAX_HP;
        } else {
            game_state->persistent.hp += hp;
        }
    }

    game_state->persistent.last_recorded_hp_update = game_events.hp_timestamp;
    if(hp != 0) {
        FURI_LOG_D(LOG_TAG, "apply_hp(): new total is %lu HP", game_state->persistent.hp);
        return true;
    }
    return false;
}

static int get_text(const struct GameState* game_state, char* str, size_t size) {
    return snprintf(str, size, "HP: %lu/%d\n", game_state->persistent.hp, MAX_HP);
}

struct Feat init_feat_health() {
    struct Feat feat;
    feat.init = init;
    feat.check = check;
    feat.apply = apply;
    feat.get_text = get_text;
    return feat;
}
