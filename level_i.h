#pragma once
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

Level* level_alloc(const LevelBehaviour* behaviour);

void level_free(Level* level);

void level_update(Level* level, Director* director);

void level_render(Level* level, Director* director, Canvas* canvas);

void level_call_alloc(Level* level);

void level_call_free(Level* level);

void level_call_start(Level* level);

void level_call_stop(Level* level);

#ifdef __cplusplus
}
#endif