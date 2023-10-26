#include <furi.h> // For FURI_LOG_D
#include <furi_hal.h> // For RTC handling

#include "state_management.h"
#include "constants.h"
#include "feature_management.h"
#include "save_restore.h"

static uint32_t get_current_timestamp() {
    FuriHalRtcDateTime current_time;
    furi_hal_rtc_get_datetime(&current_time);
    return furi_hal_rtc_datetime_to_timestamp(&current_time);
}

static void fast_forward_state(struct GameState *game_state) {
    struct GameEvents events = { 0 };
    generate_new_random_events(game_state, &events);
    process_events(game_state, events);
}

void init_state(struct GameState *game_state) {
    // Try to load the state from the storage
    if(!load_from_file(&game_state->persistent)) {
        uint32_t current_timestamp = get_current_timestamp();

        // Failed, init the struct with default values
        game_state->persistent.stage = EGG;

        // Init every individual feature
        init_xp(game_state, current_timestamp);
    } else {
        // State loaded from file. Actualize it up to
        // the current timestamp.
        FURI_LOG_D(LOG_TAG, "Fast forwarding persisted state to current time");
        fast_forward_state(game_state);
    }
    game_state->next_animation_index = 0;
}

void persist_state(struct GameState *game_state) {
    bool result = save_to_file(&game_state->persistent);
    if (!result) {
        furi_crash("Unable to save to storage");
    }
}

static void _generate_new_random_event(uint32_t timestamp, struct GameState *game_state, struct GameEvents *game_events) {
    if (game_state->persistent.stage == DEAD) {
        FURI_LOG_D(LOG_TAG, "Received generate request, but stage is DEAD");
        // Can't do much
        return;
    }
    // Check every individual feature
    check_xp(game_state, timestamp, game_events);
}

void generate_new_random_events(struct GameState *game_state, struct GameEvents *game_events) {
    uint32_t current_timestamp = get_current_timestamp();
    _generate_new_random_event(current_timestamp, game_state, game_events);
    return;
}

bool process_events(struct GameState *game_state, struct GameEvents game_events) {
    bool new_events = false;

    // Process every individual feature
    new_events |= apply_xp(game_state, game_events);

    return new_events;
}
