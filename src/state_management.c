#include <furi_hal.h> // For RTC handling

#include "constants.h"
#include "state_management.h"
#include "save_restore.h"

static uint32_t get_current_timestamp() {
    FuriHalRtcDateTime current_time;
    furi_hal_rtc_get_datetime(&current_time);
    return furi_hal_rtc_datetime_to_timestamp(&current_time);
}

void init_state(struct GameState *game_state) {
    // Try to load the state from the storage
    if(!load_from_file(&game_state->persistent)) {
        // Failed, init the struct with default values
        game_state->persistent.last_recorded_event = get_current_timestamp();
        game_state->persistent.stage = EGG;
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
    UNUSED(timestamp);
    UNUSED(game_state);
    UNUSED(game_events);
    return;
}

void generate_new_random_events(struct GameState *game_state, struct GameEvents *game_events) {
    uint32_t current_timestamp = get_current_timestamp();
    _generate_new_random_event(current_timestamp, game_state, game_events);
    return;
}

bool process_events(struct GameState *game_state, struct GameEvents game_events) {
    UNUSED(game_state);
    UNUSED(game_events);
    return false;
}
