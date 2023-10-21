#ifndef __SAVE_RESTORE_H__
#define __SAVE_RESTORE_H__

#include "game_structs.h"

bool save_to_file(struct PersistentGameState *);
bool load_from_file(struct PersistentGameState *);

#endif
