#pragma once
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LevelManager LevelManager;

Level* level_manager_add_level(LevelManager* manager);

void level_manager_next_level_set(LevelManager* manager, Level* level);

Level* level_manager_current_level_get(LevelManager* manager);

#ifdef __cplusplus
}
#endif