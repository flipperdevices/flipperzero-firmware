#pragma once
#include "level.h"
#include "game_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameManager GameManager;

Level* game_manager_add_level(GameManager* manager, const LevelBehaviour* behaviour);

void game_manager_next_level_set(GameManager* manager, Level* level);

Level* game_manager_current_level_get(GameManager* manager);

RunningGameEngine* game_manager_engine_get(GameManager* manager);

InputState game_manager_input_get(GameManager* manager);

void* game_manager_game_context_get(GameManager* manager);

#ifdef __cplusplus
}
#endif