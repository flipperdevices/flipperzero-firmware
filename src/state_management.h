#ifndef __STATE_MANAGEMENT_H__
#define __STATE_MANAGEMENT_H__

#include <furi.h>

#include "game_structs.h"

void init_state(struct GameState *);
void persist_state(struct GameState *);
void generate_new_random_events(struct GameState *, struct GameEvents *);
/* Returns true if the game state has been updated */
bool process_events(struct GameState *, struct GameEvents);

#endif
