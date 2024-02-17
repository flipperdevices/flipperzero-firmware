#pragma once
#include "level.h"
#include "game_engine.h"
#include "sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameManager GameManager;

Level* game_manager_add_level(GameManager* manager, const LevelBehaviour* behaviour);

void game_manager_next_level_set(GameManager* manager, Level* level);

Level* game_manager_current_level_get(GameManager* manager);

GameEngine* game_manager_engine_get(GameManager* manager);

InputState game_manager_input_get(GameManager* manager);

void* game_manager_game_context_get(GameManager* manager);

void game_manager_game_stop(GameManager* manager);

void game_manager_show_fps_set(GameManager* manager, bool show_fps);

/**
 * @brief Load a sprite from a file
 * Sprite will be cached and reused if the same file is loaded again
 * 
 * @param manager game manager instance
 * @param path sprite file path, relative to the game's assets folder
 * @return Sprite* or NULL if the sprite could not be loaded
 */
Sprite* game_manager_sprite_load(GameManager* manager, const char* path);

#ifdef __cplusplus
}
#endif