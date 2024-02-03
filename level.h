#pragma once
#include "entity.h"
#include "director.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Level Level;

typedef struct {
    void (*start)(Level* level, void* context);
    void (*stop)(Level* level, void* context);
    void (*update_pre)(Level* level, Director* director, void* context);
    void (*update_post)(Level* level, Director* director, void* context);
    void (*render_pre)(Level* level, Director* director, Canvas* canvas, void* context);
    void (*render_post)(Level* level, Director* director, Canvas* canvas, void* context);
} LevelBehaviour;

#define LEVEL_BEHAVIOUR_EMPTY ((LevelBehaviour){NULL, NULL, NULL, NULL, NULL, NULL})

void level_behaviour_set(Level* level, LevelBehaviour behaviour, void* context);

Entity* level_add_entity(Level* level, const EntityDescription* behaviour);

void level_remove_entity(Level* level, Entity* entity);

#ifdef __cplusplus
}
#endif