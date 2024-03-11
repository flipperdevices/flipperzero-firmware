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
#define LEVEL_INFO(...) FURI_LOG_I("Level", __VA_ARGS__)
#define LEVEL_ERROR(...) FURI_LOG_E("Level", __VA_ARGS__)

struct Level {
    EntityList_t entities;
    EntityList_t to_add;
    EntityList_t to_remove;
    const LevelBehaviour* behaviour;
    void* context;
    GameManager* manager;

    bool clear;
    LevelClearCallback clear_callback;
    void* clear_context;
};

Level* level_alloc(const LevelBehaviour* behaviour, GameManager* manager) {
    Level* level = malloc(sizeof(Level));
    level->manager = manager;
    level->clear = false;
    EntityList_init(level->entities);
    EntityList_init(level->to_add);
    EntityList_init(level->to_remove);
    level->behaviour = behaviour;
    if(behaviour->context_size > 0) {
        level->context = malloc(behaviour->context_size);
    } else {
        level->context = NULL;
    }
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

static bool level_entity_in_list_p(const EntityList_t list, Entity* entity) {
    FOREACH(item, list) {
        if(*item == entity) {
            return true;
        }
    }
    return false;
}

static void level_clear_entities(Level* level) {
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
    } while(!EntityList_empty_p(level->to_add) || !EntityList_empty_p(level->to_remove) ||
            !EntityList_empty_p(level->entities));
}

void level_free(Level* level) {
    level_clear_entities(level);

    EntityList_clear(level->entities);
    EntityList_clear(level->to_add);
    EntityList_clear(level->to_remove);

    if(level->behaviour->context_size > 0) {
        free(level->context);
    }

    LEVEL_DEBUG("Freeing level at %p", level);
    free(level);
}

Entity* level_add_entity(Level* level, const EntityDescription* description) {
    Entity* entity = entity_alloc(description);
    EntityList_push_back(level->to_add, entity);
    entity_call_start(entity, level->manager);
    return entity;
}

void level_remove_entity(Level* level, Entity* entity) {
    EntityList_push_back(level->to_remove, entity);
    entity_call_stop(entity, level->manager);
}

void level_send_event(
    Level* level,
    Entity* sender,
    const EntityDescription* receiver_desc,
    uint32_t type,
    EntityEventValue value) {
    FOREACH(item, level->entities) {
        if(receiver_desc == entity_description_get(*item) || receiver_desc == NULL) {
            entity_send_event(sender, *item, level->manager, type, value);
        }
    }

    FOREACH(item, level->to_add) {
        if(receiver_desc == entity_description_get(*item) || receiver_desc == NULL) {
            entity_send_event(sender, *item, level->manager, type, value);
        }
    }
}

static void level_process_update(Level* level, GameManager* manager) {
    FOREACH(item, level->entities) {
        entity_call_update(*item, manager);
    }
}

static void level_process_collision(Level* level, GameManager* manager) {
    EntityList_it_t it_first;
    EntityList_it_t it_second;

    EntityList_it(it_first, level->entities);
    while(!EntityList_end_p(it_first)) {
        Entity* first = *EntityList_ref(it_first);
        if(entity_collider_exists(first)) {
            // start second iterator at the next entity,
            // so we don't check the same pair twice
            EntityList_it_set(it_second, it_first);
            EntityList_next(it_second);
            while(!EntityList_end_p(it_second)) {
                Entity* second = *EntityList_ref(it_second);
                if(first->collider_dirty || second->collider_dirty) {
                    if(entity_collider_exists(second)) {
                        if(entity_collider_check_collision(first, second)) {
                            entity_call_collision(first, second, manager);
                            entity_call_collision(second, first, manager);
                        }
                    }
                }
                EntityList_next(it_second);
            }
        }
        EntityList_next(it_first);
    }

    FOREACH(item, level->entities) {
        (*item)->collider_dirty = false;
    }
}

void level_clear(Level* level, LevelClearCallback callback, void* context) {
    level->clear_callback = callback;
    level->clear_context = context;
    level->clear = true;
}

void level_update(Level* level, GameManager* manager) {
    if(level->clear) {
        level_clear_entities(level);
        level->clear_callback(level, level->clear_context);
        level->clear = false;
    }

    level_process_add(level);
    level_process_remove(level);
    level_process_update(level, manager);
    level_process_collision(level, manager);
}

void level_render(Level* level, GameManager* manager, Canvas* canvas) {
    FOREACH(item, level->entities) {
        entity_call_render(*item, manager, canvas);
    }
}

void level_call_start(Level* level) {
    if(level->behaviour->start) {
        level->behaviour->start(level, level->manager, level->context);
    }
}

void level_call_stop(Level* level) {
    if(level->behaviour->stop) {
        level->behaviour->stop(level, level->manager, level->context);
    }
}

void level_call_alloc(Level* level) {
    if(level->behaviour->alloc) {
        level->behaviour->alloc(level, level->manager, level->context);
    }
}

void level_call_free(Level* level) {
    if(level->behaviour->free) {
        level->behaviour->free(level, level->manager, level->context);
    }
}

size_t level_entity_count(const Level* level, const EntityDescription* description) {
    size_t count = 0;
    FOREACH(item, level->entities) {
        if(description == NULL || description == entity_description_get(*item)) {
            count++;
        }
    }

    // subtract entities that are in to_remove
    FOREACH(item, level->to_remove) {
        if(description == NULL || description == entity_description_get(*item)) {
            count--;
        }
    }

    // add entities that are in to_add
    FOREACH(item, level->to_add) {
        if(description == NULL || description == entity_description_get(*item)) {
            count++;
        }
    }

    return count;
}

Entity* level_entity_get(const Level* level, const EntityDescription* description, size_t index) {
    size_t count = 0;
    FOREACH(item, level->entities) {
        if(description == NULL || description == entity_description_get(*item)) {
            if(!level_entity_in_list_p(level->to_remove, *item)) {
                if(count == index) {
                    return *item;
                }
                count++;
            }
        }
    }

    FOREACH(item, level->to_add) {
        if(description == NULL || description == entity_description_get(*item)) {
            if(!level_entity_in_list_p(level->to_remove, *item)) {
                if(count == index) {
                    return *item;
                }
                count++;
            }
        }
    }

    return NULL;
}

void* level_context_get(Level* level) {
    return level->context;
}

bool level_contains_entity(const Level* level, const Entity* entity) {
    FOREACH(item, level->entities) {
        if(*item == entity) {
            return true;
        }
    }
    FOREACH(item, level->to_add) {
        if(*item == entity) {
            return true;
        }
    }
    return false;
}