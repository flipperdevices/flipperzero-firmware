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

typedef struct {
    void (*start)(void* context);
    void (*stop)(void* context);
    void (*update)(Entity* entity, Director* director, void* context);
    void (*render)(Entity* entity, Director* director, Canvas* canvas, void* context);
    size_t context_size;
} EntityBehaviour;

Vector entity_pos_get(Entity* entity);

void entity_pos_set(Entity* entity, Vector position);

void* entity_context_get(Entity* entity);

#ifdef __cplusplus
}
#endif