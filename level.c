#include "level.h"
#include "level_i.h"
#include "entity_i.h"
#include <m-list.h>
#include <furi.h>

LIST_DEF(EntityList, Entity*, M_POD_OPLIST);
#define M_OPL_EntityList_t() LIST_OPLIST(EntityList)
#define FOREACH(name, list) for \
    M_EACH(name, list, EntityList_t)

#define LEVEL_DEBUG(...) FURI_LOG_D("Level", __VA_ARGS__)
#define LEVEL_ERROR(...) FURI_LOG_E("Level", __VA_ARGS__)

struct Level {
    EntityList_t entities;
    EntityList_t to_add;
    EntityList_t to_remove;
    LevelBehaviour behaviour;
    void* context;
};

Level* level_alloc(void) {
    Level* level = malloc(sizeof(Level));
    EntityList_init(level->entities);
    EntityList_init(level->to_add);
    EntityList_init(level->to_remove);
    level->behaviour = LEVEL_BEHAVIOUR_EMPTY;
    level->context = NULL;
    LEVEL_DEBUG("Allocated level at %p", level);
    return level;
}

static void level_process_add(Level* level) {
    // move entities from to_add to entities
    FOREACH(item, level->to_add) {
        EntityList_push_back(level->entities, *item);
    }
    EntityList_clear(level->to_add);
}

static void level_process_remove(Level* level) {
    // remove entities in to_remove from entities and free them
    FOREACH(item, level->to_remove) {
        entity_free(*item);
        EntityList_it_t it;

        // find and remove the entity from the entities list
        for(EntityList_it(it, level->entities); !EntityList_end_p(it); EntityList_next(it)) {
            if(*EntityList_ref(it) == *item) {
                EntityList_remove(level->entities, it);
                break;
            }
        }
    }
    EntityList_clear(level->to_remove);
}

static bool level_entity_in_list_p(EntityList_t list, Entity* entity) {
    FOREACH(item, list) {
        if(*item == entity) {
            return true;
        }
    }
    return false;
}

void level_free(Level* level) {
    size_t iterations = 0;

    do {
        // process to_add and to_remove
        level_process_add(level);
        level_process_remove(level);

        // remove entities from entities list
        FOREACH(item, level->entities) {
            if(!level_entity_in_list_p(level->to_remove, *item)) {
                level_remove_entity(level, *item);
            }
        }

        // check if we are looping too many times
        iterations++;
        if(iterations >= 100) {
            LEVEL_ERROR("Level free looped too many times");
        }

        // entity_call_stop can call level_remove_entity or level_add_entity
        // so we need to process to_add and to_remove again
    } while(!EntityList_empty_p(level->to_add) || !EntityList_empty_p(level->to_remove));

    EntityList_clear(level->entities);
    EntityList_clear(level->to_add);
    EntityList_clear(level->to_remove);

    LEVEL_DEBUG("Freeing level at %p", level);
    free(level);
}

void level_behaviour_set(Level* level, LevelBehaviour behaviour, void* context) {
    level->behaviour = behaviour;
    level->context = context;
}

Entity* level_add_entity(Level* level, const EntityDescription* behaviour) {
    Entity* entity = entity_alloc(behaviour);
    EntityList_push_back(level->to_add, entity);
    entity_call_start(level, entity);
    return entity;
}

void level_remove_entity(Level* level, Entity* entity) {
    EntityList_push_back(level->to_remove, entity);
    entity_call_stop(level, entity);
}

void level_update(Level* level, Director* director) {
    level_process_add(level);
    level_process_remove(level);

    FOREACH(item, level->entities) {
        entity_call_update(*item, director);
    }
}

void level_render(Level* level, Director* director, Canvas* canvas) {
    FOREACH(item, level->entities) {
        entity_call_render(*item, director, canvas);
    }
}