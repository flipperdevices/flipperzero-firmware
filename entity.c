#include "entity.h"
#include "entity_i.h"
#include <stdlib.h>
#include <furi.h>

#define ENTITY_DEBUG(...) FURI_LOG_D("Entity", __VA_ARGS__)

static int32_t entities_count = 0;

int32_t entities_get_count(void) {
    return entities_count;
}

struct Entity {
    Vector position;
    const EntityDescription* behaviour;
    void* context;
};

Entity* entity_alloc(const EntityDescription* behaviour) {
    entities_count++;
    Entity* entity = malloc(sizeof(Entity));
    entity->position = VECTOR_ZERO;
    entity->behaviour = behaviour;
    entity->context = NULL;
    if(behaviour && behaviour->context_size > 0) {
        entity->context = malloc(behaviour->context_size);
    }
    ENTITY_DEBUG("Allocated at %p", entity);
    return entity;
}

void entity_free(Entity* entity) {
    entities_count--;
    ENTITY_DEBUG("Freeing at %p", entity);
    if(entity->context) {
        free(entity->context);
    }
    free(entity);
}

Vector entity_pos_get(Entity* entity) {
    return entity->position;
}

void entity_pos_set(Entity* entity, Vector position) {
    entity->position = position;
}

void* entity_context_get(Entity* entity) {
    return entity->context;
}

void entity_call_start(Level* level, Entity* entity) {
    if(entity->behaviour && entity->behaviour->start) {
        entity->behaviour->start(entity, level, entity->context);
    }
}

void entity_call_stop(Level* level, Entity* entity) {
    if(entity->behaviour && entity->behaviour->stop) {
        entity->behaviour->stop(entity, level, entity->context);
    }
}

void entity_call_update(Entity* entity, Director* director) {
    if(entity->behaviour && entity->behaviour->update) {
        entity->behaviour->update(entity, director, entity->context);
    }
}

void entity_call_render(Entity* entity, Director* director, Canvas* canvas) {
    if(entity->behaviour && entity->behaviour->render) {
        entity->behaviour->render(entity, director, canvas, entity->context);
    }
}