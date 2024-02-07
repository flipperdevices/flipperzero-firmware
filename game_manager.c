#include "game_manager.h"
#include "level_i.h"
#include <furi.h>
#include <m-list.h>

LIST_DEF(LevelList, Level*, M_POD_OPLIST);

struct GameManager {
    LevelList_t levels;
    Level* current_level;
    Level* next_level;

    RunningGameEngine* engine;
    InputState input;
    void* game_context;
};

GameManager* game_manager_alloc() {
    GameManager* manager = malloc(sizeof(GameManager));
    LevelList_init(manager->levels);
    manager->current_level = NULL;
    manager->next_level = NULL;
    manager->engine = NULL;
    manager->game_context = NULL;
    memset(&manager->input, 0, sizeof(InputState));
    return manager;
}

void game_manager_free(GameManager* manager) {
    level_call_stop(manager->current_level);

    LevelList_it_t it;
    LevelList_it(it, manager->levels);
    while(!LevelList_end_p(it)) {
        level_call_free(*LevelList_cref(it));
        level_free(*LevelList_cref(it));
        LevelList_next(it);
    }

    LevelList_clear(manager->levels);
    free(manager);
}

Level* game_manager_add_level(GameManager* manager, const LevelBehaviour* behaviour) {
    UNUSED(manager);
    Level* level = level_alloc(behaviour);
    LevelList_push_back(manager->levels, level);
    level_call_alloc(level);
    if(!manager->current_level) {
        manager->current_level = level;
        level_call_start(level);
    }
    return level;
}

void game_manager_next_level_set(GameManager* manager, Level* next_level) {
    manager->next_level = next_level;
}

Level* game_manager_current_level_get(GameManager* manager) {
    return manager->current_level;
}

void game_manager_update(GameManager* manager) {
    if(manager->next_level) {
        level_call_stop(manager->current_level);
        manager->current_level = manager->next_level;
        level_call_start(manager->current_level);
        manager->next_level = NULL;
    }

    level_update(manager->current_level, manager);
}

void game_manager_render(GameManager* manager, Canvas* canvas) {
    level_render(manager->current_level, manager, canvas);
}

void game_manager_engine_set(GameManager* manager, RunningGameEngine* engine) {
    manager->engine = engine;
}

void game_manager_input_set(GameManager* manager, InputState input) {
    manager->input = input;
}

void game_manager_game_context_set(GameManager* manager, void* context) {
    manager->game_context = context;
}

RunningGameEngine* game_manager_engine_get(GameManager* manager) {
    return manager->engine;
}

InputState game_manager_input_get(GameManager* manager) {
    return manager->input;
}

void* game_manager_game_context_get(GameManager* manager) {
    return manager->game_context;
}