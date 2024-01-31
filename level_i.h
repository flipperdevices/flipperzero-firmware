#pragma once
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

Level* level_alloc(void);

void level_free(Level* level);

void level_update(Level* level, Director* director);

void level_render(Level* level, Director* director, Canvas* canvas);

#ifdef __cplusplus
}
#endif