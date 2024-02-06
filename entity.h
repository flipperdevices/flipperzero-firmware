#pragma once
#include "game_engine.h"
#include "director.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Entity Entity;

typedef struct {
    float x;
    float y;
} Vector;

#define VECTOR_ZERO ((Vector){0, 0})

typedef union {
    uint32_t value;
    void* pointer;
} EntityEventValue;

typedef struct {
    uint32_t type;
    Entity* sender;
    EntityEventValue value;
} EntityEvent;

typedef struct {
    void (*start)(Entity* self, Level* level, void* context);
    void (*stop)(Entity* self, Level* level, void* context);
    void (*update)(Entity* self, Director* director, void* context);
    void (*render)(Entity* self, Director* director, Canvas* canvas, void* context);
    void (*collision)(Entity* self, Entity* other, Director* director, void* context);
    void (*event)(Entity* self, Director* director, EntityEvent event, void* context);
    size_t context_size;
} EntityDescription;

const EntityDescription* entity_description_get(Entity* entity);

Vector entity_pos_get(Entity* entity);

void entity_pos_set(Entity* entity, Vector position);

void* entity_context_get(Entity* entity);

void entity_collider_add_circle(Entity* entity, float radius);

void entity_collider_add_rect(Entity* entity, float width, float height);

void entity_collider_offset_set(Entity* entity, Vector offset);

Vector entity_collider_offset_get(Entity* entity);

void entity_send_event(Entity* entity, uint32_t type, EntityEventValue value);

#ifdef __cplusplus
}
#endif