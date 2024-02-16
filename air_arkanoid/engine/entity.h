#pragma once
#include "vector.h"
#include "game_engine.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Entity Entity;

typedef struct Level Level;

typedef union {
    uint32_t value;
    void* pointer;
} EntityEventValue;

typedef struct {
    uint32_t type;
    Entity* sender;
    EntityEventValue value;
} EntityEvent;

typedef struct Level Level;
typedef struct GameManager GameManager;

typedef struct {
    void (*start)(Entity* self, GameManager* manager, void* context);
    void (*stop)(Entity* self, GameManager* manager, void* context);
    void (*update)(Entity* self, GameManager* manager, void* context);
    void (*render)(Entity* self, GameManager* manager, Canvas* canvas, void* context);
    void (*collision)(Entity* self, Entity* other, GameManager* manager, void* context);
    void (*event)(Entity* self, GameManager* manager, EntityEvent event, void* context);
    size_t context_size;
} EntityDescription;

const EntityDescription* entity_description_get(Entity* entity);

Vector entity_pos_get(Entity* entity);

void entity_pos_set(Entity* entity, Vector position);

void* entity_context_get(Entity* entity);

void entity_collider_add_circle(Entity* entity, float radius);

void entity_collider_add_rect(Entity* entity, float width, float height);

void entity_collider_remove(Entity* entity);

void entity_collider_offset_set(Entity* entity, Vector offset);

Vector entity_collider_offset_get(Entity* entity);

void entity_send_event(
    Entity* sender,
    Entity* receiver,
    GameManager* manager,
    uint32_t type,
    EntityEventValue value);

#ifdef __cplusplus
}
#endif