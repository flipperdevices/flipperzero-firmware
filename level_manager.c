#include "level_manager.h"
#include "level_i.h"
#include <furi.h>
#include <m-list.h>

LIST_DEF(LevelList, Level*, M_POD_OPLIST);

struct LevelManager {
    LevelList_t levels;
    Level* current_level;
    Level* next_level;
};

LevelManager* level_manager_alloc() {
    LevelManager* lm = malloc(sizeof(LevelManager));
    lm->current_level = NULL;
    lm->next_level = NULL;
    LevelList_init(lm->levels);
    return lm;
}

void level_manager_free(LevelManager* lm) {
    LevelList_it_t it;
    LevelList_it(it, lm->levels);
    while(!LevelList_end_p(it)) {
        level_free(*LevelList_cref(it));
        LevelList_next(it);
    }

    LevelList_clear(lm->levels);
    free(lm);
}

Level* level_manager_add_level(LevelManager* manager) {
    UNUSED(manager);
    Level* level = level_alloc();
    LevelList_push_back(manager->levels, level);
    if(!manager->current_level) {
        manager->current_level = level;
    }
    return level;
}

void level_manager_next_level_set(LevelManager* lm, Level* next_level) {
    lm->next_level = next_level;
}

Level* level_manager_current_level_get(LevelManager* lm) {
    return lm->current_level;
}

void level_manager_update(LevelManager* lm, Director* director) {
    if(lm->next_level) {
        lm->current_level = lm->next_level;
        lm->next_level = NULL;
    }
    level_update(lm->current_level, director);
}

void level_manager_render(LevelManager* lm, Director* director, Canvas* canvas) {
    level_render(lm->current_level, director, canvas);
}