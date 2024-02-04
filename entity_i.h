#pragma once
#include "entity.h"
#include "director.h"

#ifdef __cplusplus
extern "C" {
#endif

Entity* entity_alloc(const EntityDescription* behaviour);

void entity_free(Entity* entity);

void entity_call_start(Level* level, Entity* entity);

void entity_call_stop(Level* level, Entity* entity);

void entity_call_update(Entity* entity, Director* director);

void entity_call_render(Entity* entity, Director* director, Canvas* canvas);

void entity_call_collision(Entity* entity, Entity* other, Director* director);

bool entity_collider_check_collision(Entity* entity, Entity* other);

bool entity_collider_exists(Entity* entity);

int32_t entities_get_count(void);

#ifdef __cplusplus
}
#endif