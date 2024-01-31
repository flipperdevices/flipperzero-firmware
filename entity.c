#include "entity.h"
#include "entity_i.h"
#include <stdlib.h>
#include <furi.h>

struct Entity {
    Vector position;
    EntityBehaviour behaviour;
    void* context;
};

Entity* entity_alloc() {
    Entity* entity = malloc(sizeof(Entity));
    entity->position = VECTOR_ZERO;
    entity->behaviour = ENTITY_BEHAVIOUR_EMPTY;
    entity->context = NULL;
    FURI_LOG_D("Entity", "Allocated entity at %p", entity);
    return entity;
}

void entity_free(Entity* entity) {
    FURI_LOG_D("Entity", "Freeing entity at %p", entity);
    free(entity);
}

void entity_behaviour_set(Entity* entity, EntityBehaviour behaviour) {
    entity->behaviour = behaviour;
}

void entity_context_set(Entity* entity, void* context) {
    entity->context = context;
}

Vector entity_pos_get(Entity* entity) {
    return entity->position;
}

void entity_pos_set(Entity* entity, Vector position) {
    entity->position = position;
}

void entity_pos_add(Entity* entity, Vector delta) {
    entity->position.x += delta.x;
    entity->position.y += delta.y;
}

void* entity_context_get(Entity* entity) {
    return entity->context;
}

void entity_call_start(Entity* entity) {
    if(entity->behaviour.start) {
        entity->behaviour.start(entity);
    }
}

void entity_call_stop(Entity* entity) {
    if(entity->behaviour.stop) {
        entity->behaviour.stop(entity->context);
    }
}

void entity_call_update(Entity* entity, Director* director) {
    if(entity->behaviour.update) {
        entity->behaviour.update(entity, director, entity->context);
    }
}

void entity_call_render(Entity* entity, Director* director, Canvas* canvas) {
    if(entity->behaviour.render) {
        entity->behaviour.render(entity, director, canvas, entity->context);
    }
}