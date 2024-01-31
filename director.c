#include "director.h"
#include "director_i.h"
#include "level_manager.h"
#include <stdlib.h>
#include <string.h>

struct Director {
    RunningGameEngine* engine;
    LevelManager* level_manager;
    InputState input;
};

Director* director_alloc() {
    Director* director = malloc(sizeof(Director));
    memset(director, 0, sizeof(Director));
    return director;
}

void director_free(Director* director) {
    free(director);
}

void director_input_set(Director* director, InputState input) {
    director->input = input;
}

void director_engine_set(Director* director, RunningGameEngine* engine) {
    director->engine = engine;
}

void director_level_manager_set(Director* director, LevelManager* level_manager) {
    director->level_manager = level_manager;
}

InputState director_input_get(Director* director) {
    return director->input;
}

RunningGameEngine* director_engine_get(Director* director) {
    return director->engine;
}

LevelManager* director_level_manager_get(Director* director) {
    return director->level_manager;
}

Level* director_level_get(Director* director) {
    return level_manager_current_level_get(director->level_manager);
}
