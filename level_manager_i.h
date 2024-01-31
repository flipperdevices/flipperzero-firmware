#pragma once
#include "level_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

LevelManager* level_manager_alloc(void);

void level_manager_free(LevelManager* manager);

void level_manager_update(LevelManager* level, Director* director);

void level_manager_render(LevelManager* level, Director* director, Canvas* canvas);

#ifdef __cplusplus
}
#endif