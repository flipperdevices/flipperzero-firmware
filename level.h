#pragma once
#include <stddef.h>
#include "entity.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameManager GameManager;

typedef struct {
    void (*alloc)(Level* level, GameManager* manager, void* context);
    void (*free)(Level* level, GameManager* manager, void* context);
    void (*start)(Level* level, GameManager* manager, void* context);
    void (*stop)(Level* level, GameManager* manager, void* context);
    size_t context_size;
} LevelBehaviour;

void level_clear(Level* level);

Entity* level_add_entity(Level* level, const EntityDescription* behaviour);

void level_remove_entity(Level* level, Entity* entity);

#ifdef __cplusplus
}
#endif