#include "level.h"
#include "level_i.h"
#include "entity_i.h"
#include <m-list.h>
#include <furi.h>

LIST_DEF(EntityList, Entity*, M_POD_OPLIST);

struct Level {
    EntityList_t entities;
    EntityList_t entities_to_add;
    EntityList_t entities_to_remove;
    LevelBehaviour behaviour;
    void* context;
};

static void level_process_add(Level* level) {
    EntityList_it_t it;
    EntityList_it(it, level->entities_to_add);
    while(!EntityList_end_p(it)) {
        EntityList_push_back(level->entities, *EntityList_cref(it));
        entity_call_start(*EntityList_ref(it));
        EntityList_next(it);
    }
    EntityList_clear(level->entities_to_add);
}

static void level_find_and_remove(Level* level, Entity* entity) {
    EntityList_it_t it;
    EntityList_it(it, level->entities);
    while(!EntityList_end_p(it)) {
        if(*EntityList_ref(it) == entity) {
            entity_call_stop(*EntityList_ref(it));
            EntityList_remove(level->entities, it);
            entity_free(entity);
            break;
        }
        EntityList_next(it);
    }
}

static void level_process_remove(Level* level) {
    EntityList_it_t it;
    EntityList_it(it, level->entities_to_remove);
    while(!EntityList_end_p(it)) {
        level_find_and_remove(level, *EntityList_cref(it));
        EntityList_next(it);
    }
    EntityList_clear(level->entities_to_remove);
}

Level* level_alloc(void) {
    Level* level = malloc(sizeof(Level));
    EntityList_init(level->entities);
    EntityList_init(level->entities_to_add);
    EntityList_init(level->entities_to_remove);
    level->behaviour = LEVEL_BEHAVIOUR_EMPTY;
    level->context = NULL;
    FURI_LOG_D("Level", "Allocated level at %p", level);
    return level;
}

void level_free(Level* level) {
    level_process_add(level);
    level_process_remove(level);

    EntityList_it_t it;
    EntityList_it(it, level->entities);
    while(!EntityList_end_p(it)) {
        entity_call_stop(*EntityList_ref(it));
        entity_free(*EntityList_ref(it));
        EntityList_next(it);
    }

    EntityList_clear(level->entities);
    EntityList_clear(level->entities_to_add);
    EntityList_clear(level->entities_to_remove);

    FURI_LOG_D("Level", "Freeing level at %p", level);
    free(level);
}

void level_behaviour_set(Level* level, LevelBehaviour behaviour, void* context) {
    level->behaviour = behaviour;
    level->context = context;
}

Entity* level_add_entity(Level* level, const EntityDescription* behaviour) {
    Entity* entity = entity_alloc(behaviour);
    EntityList_push_back(level->entities_to_add, entity);
    return entity;
}

void level_remove_entity(Level* level, Entity* entity) {
    EntityList_push_back(level->entities_to_remove, entity);
}

void level_update(Level* level, Director* director) {
    level_process_add(level);
    level_process_remove(level);

    EntityList_it_t it;
    EntityList_it(it, level->entities);
    while(!EntityList_end_p(it)) {
        entity_call_update(*EntityList_ref(it), director);
        EntityList_next(it);
    }
}

void level_render(Level* level, Director* director, Canvas* canvas) {
    EntityList_it_t it;
    EntityList_it(it, level->entities);
    while(!EntityList_end_p(it)) {
        entity_call_render(*EntityList_ref(it), director, canvas);
        EntityList_next(it);
    }
}