#ifndef __SETTINGS_MANAGEMENT_H__
#define __SETTINGS_MANAGEMENT_H__

#include "game_structs.h"

void init_settings(struct GameState *);
void persist_settings(struct GameState *);

// Vibration
void vibrate_short(const struct GameState *);

// Sound

void play_settings_confirm(const struct GameState *);

#endif
