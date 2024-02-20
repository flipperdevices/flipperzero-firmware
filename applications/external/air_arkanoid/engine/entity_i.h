#pragma once
#include "entity.h"
#include "game_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ColliderTypeCircle,
    ColliderTypeRect,
} ColliderType;

typedef struct {
    ColliderType type;
    union {
        struct {
            float radius;
        } circle;
        struct {
            float half_width;
            float half_height;
        } rect;
    };
} Collider;

struct Entity {
    Vector position;
    const EntityDescription* description;
    void* context;
    Collider* collider;
    Vector collider_offset;
    bool collider_dirty;
};

Entity* entity_alloc(const EntityDescription* behaviour);

void entity_free(Entity* entity);

void entity_call_start(Entity* entity, GameManager* manager);

void entity_call_stop(Entity* entity, GameManager* manager);

void entity_call_update(Entity* entity, GameManager* manager);

void entity_call_render(Entity* entity, GameManager* manager, Canvas* canvas);

void entity_call_collision(Entity* entity, Entity* other, GameManager* manager);

bool entity_collider_check_collision(Entity* entity, Entity* other);

bool entity_collider_exists(Entity* entity);

int32_t entities_get_count(void);

#ifdef __cplusplus
}
#endif