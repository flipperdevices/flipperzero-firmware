#ifndef __STATE_MANAGEMENT_H__
#define __STATE_MANAGEMENT_H__

#include <stdint.h>

#include "game_structs.h"

void init_state(struct GameState *);
void persist_state(struct GameState *);
void reset_state(struct GameState *);
void generate_new_random_events(struct GameState *, struct GameEvents *);
/* Returns true if the game state has been updated */
bool process_events(struct GameState *, struct GameEvents);
/* Get a string that can be used to show the game state */
void get_state_str(const struct GameState *, char *, size_t);

#endif
