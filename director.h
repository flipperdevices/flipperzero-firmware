#pragma once
#include "game_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Director Director;
typedef struct RunningGameEngine RunningGameEngine;
typedef struct LevelManager LevelManager;
typedef struct Level Level;

InputState director_input_get(Director* director);

RunningGameEngine* director_engine_get(Director* director);

LevelManager* director_level_manager_get(Director* director);

Level* director_level_get(Director* director);

#ifdef __cplusplus
}
#endif