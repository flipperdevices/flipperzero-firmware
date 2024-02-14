#pragma once
#include "game_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

GameManager* game_manager_alloc(void);

void game_manager_free(GameManager* manager);

void game_manager_update(GameManager* manager);

void game_manager_render(GameManager* manager, Canvas* canvas);

void game_manager_engine_set(GameManager* manager, GameEngine* engine);

void game_manager_input_set(GameManager* manager, InputState input);

void game_manager_game_context_set(GameManager* manager, void* context);

#ifdef __cplusplus
}
#endif