#pragma once
#include "director.h"

#ifdef __cplusplus
extern "C" {
#endif

Director* director_alloc();

void director_free(Director* director);

void director_input_set(Director* director, InputState input);

void director_engine_set(Director* director, RunningGameEngine* engine);

void director_level_manager_set(Director* director, LevelManager* level_manager);

#ifdef __cplusplus
}
#endif