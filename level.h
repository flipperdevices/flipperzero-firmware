#pragma once
#include <stddef.h>
#include "entity.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*alloc)(Level* level, void* context);
    void (*free)(Level* level, void* context);
    void (*start)(Level* level, void* context);
    void (*stop)(Level* level, void* context);
    size_t context_size;
} LevelBehaviour;

void level_clear(Level* level);

Entity* level_add_entity(Level* level, const EntityDescription* behaviour);

void level_remove_entity(Level* level, Entity* entity);

#ifdef __cplusplus
}
#endif