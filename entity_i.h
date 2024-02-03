#pragma once
#include "entity.h"
#include "director.h"

#ifdef __cplusplus
extern "C" {
#endif

Entity* entity_alloc(const EntityDescription* behaviour);

void entity_free(Entity* entity);

void entity_call_start(Entity* entity);

void entity_call_stop(Entity* entity);

void entity_call_update(Entity* entity, Director* director);

void entity_call_render(Entity* entity, Director* director, Canvas* canvas);

#ifdef __cplusplus
}
#endif