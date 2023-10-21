#ifndef __STATE_MANAGEMENT_H__
#define __STATE_MANAGEMENT_H__

#include <furi.h>

#include "game_structs.h"

void init_state(struct GameState *);
void persist_state(struct GameState *);

#endif
