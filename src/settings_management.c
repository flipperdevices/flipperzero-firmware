#include <furi.h> // For FURI_LOG_D

#include <furi_hal.h>

#include <notification/notification_messages.h>
#include <notification/notification.h>

#include "settings_management.h"
#include "save_restore.h"
#include "game_structs.h"

#define VIBRATION_ENABLED(game_state) game_state->settings.vibration
#define SOUND_ENABLED(game_state) game_state->settings.sound

static void init_persistent_settings_object(struct GameState *game_state) {
    // Init the struct with default values
    game_state->settings.vibration = 0;
    game_state->settings.sound = 0;
}

void init_settings(struct GameState *game_state) {
    // Try to load the settings from the storage
    if (!load_settings_from_file(&game_state->settings)) {
        init_persistent_settings_object(game_state);
    }
}

void persist_settings(struct GameState *game_state) {
    bool result = save_settings_to_file(&game_state->settings);
    if (!result) {
        furi_crash("Unable to save settings to storage");
    }
}

// Vibration

static void vibrate_ms(const struct GameState *game_state, uint32_t ms) {
    if (VIBRATION_ENABLED(game_state)) {
        furi_hal_vibro_on(true);
        furi_delay_ms(ms);
        furi_hal_vibro_on(false);
    }
}

void vibrate_short(const struct GameState *game_state) {
    vibrate_ms(game_state, 100);
}

// Sound

static void play_sequence(const struct GameState *game_state, const NotificationSequence *sequence) {
    if (SOUND_ENABLED(game_state)) {
        NotificationApp *notifications = furi_record_open(RECORD_NOTIFICATION);
        notification_message(notifications, sequence);
    }
}

const NotificationSequence settings_confirm_sequence = {
    &message_note_c6,
    &message_delay_100,
    NULL
};

void play_settings_confirm(const struct GameState *game_state) {
    play_sequence(game_state, &settings_confirm_sequence);
}
