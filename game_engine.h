#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float fps;
    bool show_fps;
} GameEngineSettings;

GameEngineSettings game_engine_settings_init();

typedef struct GameEngine GameEngine;

GameEngine* game_engine_alloc(GameEngineSettings settings);

void game_engine_run(GameEngine* engine);

void game_engine_stop(GameEngine* engine);

void game_engine_free(GameEngine* engine);

#ifdef __cplusplus
}
#endif