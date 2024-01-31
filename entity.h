#pragma once
#include "game_engine.h"
#include "director.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Entity Entity;

typedef struct {
    float x;
    float y;
} Vector;

#define VECTOR_ZERO ((Vector){0, 0})

typedef struct {
    void (*start)(Entity* entity);
    void (*stop)(void* context);
    void (*update)(Entity* entity, Director* director, void* context);
    void (*render)(Entity* entity, Director* director, Canvas* canvas, void* context);
} EntityBehaviour;

#define ENTITY_BEHAVIOUR_EMPTY ((EntityBehaviour){NULL, NULL, NULL, NULL})

void entity_behaviour_set(Entity* entity, EntityBehaviour behaviour);

void entity_context_set(Entity* entity, void* context);

Vector entity_pos_get(Entity* entity);

void entity_pos_set(Entity* entity, Vector position);

void entity_pos_add(Entity* entity, Vector delta);

void* entity_context_get(Entity* entity);

#ifdef __cplusplus
}
#endif