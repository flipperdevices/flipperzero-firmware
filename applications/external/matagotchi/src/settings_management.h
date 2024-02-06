#ifndef __SETTINGS_MANAGEMENT_H__
#define __SETTINGS_MANAGEMENT_H__

#include "game_structs.h"

void init_settings(struct GameState*);
void persist_settings(struct GameState*);

/*
 * Temporary suppress vibration and sound.
 * The following two macros must be used within
 * the same scope
 */
#define MASK_VIBRO_SOUND(game_state)                    \
    uint8_t vibration = game_state->settings.vibration; \
    uint8_t sound = game_state->settings.sound;         \
    game_state->settings.vibration = 0;                 \
    game_state->settings.sound = 0;

#define UNMASK_VIBRO_SOUND(game_state)          \
    game_state->settings.vibration = vibration; \
    game_state->settings.sound = sound;

// Vibration
void vibrate_short(const struct GameState*);
void vibrate_long(const struct GameState*);

// Sound

void play_settings_confirm(const struct GameState*);
void play_level_up(const struct GameState*);
void play_starvation(const struct GameState*);
void play_ambulance(const struct GameState*);
void play_action(const struct GameState*);

#endif
