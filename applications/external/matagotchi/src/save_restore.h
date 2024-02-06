#ifndef __SAVE_RESTORE_H__
#define __SAVE_RESTORE_H__

#include "game_structs.h"

bool save_state_to_file(struct PersistentGameState*);
bool load_state_from_file(struct PersistentGameState*);
bool save_settings_to_file(struct PersistentSettings*);
bool load_settings_from_file(struct PersistentSettings*);

#endif
