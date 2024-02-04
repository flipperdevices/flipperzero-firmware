#include "entity.h"
#include "entity_i.h"
#include <stdlib.h>
#include <furi.h>

#define ENTITY_DEBUG(...) FURI_LOG_D("Entity", __VA_ARGS__)

static int32_t entities_count = 0;

int32_t entities_get_count(void) {
    return entities_count;
}

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
            float width;
            float height;
        } rect;
    };
} ColliderDescription;

struct Entity {
    Vector position;
    const EntityDescription* description;
    void* context;
    ColliderDescription* collider;
};

Entity* entity_alloc(const EntityDescription* description) {
    entities_count++;
    Entity* entity = malloc(sizeof(Entity));
    entity->position = VECTOR_ZERO;
    entity->description = description;
    entity->context = NULL;
    if(description && description->context_size > 0) {
        entity->context = malloc(description->context_size);
    }
    entity->collider = NULL;
    ENTITY_DEBUG("Allocated at %p", entity);
    return entity;
}

void entity_collider_add_circle(Entity* entity, float radius) {
    furi_check(entity->collider == NULL, "Collider already added");
    entity->collider = malloc(sizeof(ColliderDescription));
    entity->collider->type = ColliderTypeCircle;
    entity->collider->circle.radius = radius;
}

void entity_collider_add_rect(Entity* entity, float width, float height) {
    furi_check(entity->collider == NULL, "Collider already added");
    entity->collider = malloc(sizeof(ColliderDescription));
    entity->collider->type = ColliderTypeRect;
    entity->collider->rect.width = width;
    entity->collider->rect.height = height;
}

void entity_free(Entity* entity) {
    entities_count--;
    ENTITY_DEBUG("Freeing at %p", entity);
    if(entity->context) {
        free(entity->context);
    }
    if(entity->collider) {
        free(entity->collider);
    }
    free(entity);
}

const EntityDescription* entity_description_get(Entity* entity) {
    return entity->description;
}

Vector entity_pos_get(Entity* entity) {
    return entity->position;
}

void entity_pos_set(Entity* entity, Vector position) {
    entity->position = position;
}

void* entity_context_get(Entity* entity) {
    return entity->context;
}

void entity_call_start(Level* level, Entity* entity) {
    if(entity->description && entity->description->start) {
        entity->description->start(entity, level, entity->context);
    }
}

void entity_call_stop(Level* level, Entity* entity) {
    if(entity->description && entity->description->stop) {
        entity->description->stop(entity, level, entity->context);
    }
}

void entity_call_update(Entity* entity, Director* director) {
    if(entity->description && entity->description->update) {
        entity->description->update(entity, director, entity->context);
    }
}

void entity_call_render(Entity* entity, Director* director, Canvas* canvas) {
    if(entity->description && entity->description->render) {
        entity->description->render(entity, director, canvas, entity->context);
    }
}

void entity_call_collision(Entity* entity, Entity* other, Director* director) {
    if(entity->description && entity->description->collision) {
        entity->description->collision(entity, other, director, entity->context);
    }
}

bool entity_collider_circle_circle(Entity* entity, Entity* other) {
    float dx = entity->position.x - other->position.x;
    float dy = entity->position.y - other->position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < entity->collider->circle.radius + other->collider->circle.radius;
}

bool entity_collider_rect_rect(Entity* entity, Entity* other) {
    float left1 = entity->position.x - entity->collider->rect.width / 2;
    float right1 = entity->position.x + entity->collider->rect.width / 2;
    float top1 = entity->position.y - entity->collider->rect.height / 2;
    float bottom1 = entity->position.y + entity->collider->rect.height / 2;

    float left2 = other->position.x - other->collider->rect.width / 2;
    float right2 = other->position.x + other->collider->rect.width / 2;
    float top2 = other->position.y - other->collider->rect.height / 2;
    float bottom2 = other->position.y + other->collider->rect.height / 2;

    return left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2;
}

bool entity_collider_circle_rect(Entity* entity, Entity* other) {
    float left = other->position.x - other->collider->rect.width / 2;
    float right = other->position.x + other->collider->rect.width / 2;
    float top = other->position.y - other->collider->rect.height / 2;
    float bottom = other->position.y + other->collider->rect.height / 2;

    float closestX = fmaxf(left, fminf(entity->position.x, right));
    float closestY = fmaxf(top, fminf(entity->position.y, bottom));

    float dx = entity->position.x - closestX;
    float dy = entity->position.y - closestY;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < entity->collider->circle.radius;
}

bool entity_collider_check_collision(Entity* entity, Entity* other) {
    furi_check(entity->collider);
    furi_check(other->collider);

    if(entity->collider->type == ColliderTypeCircle) {
        if(other->collider->type == ColliderTypeCircle) {
            return entity_collider_circle_circle(entity, other);
        } else {
            return entity_collider_circle_rect(entity, other);
        }
    } else {
        if(other->collider->type == ColliderTypeCircle) {
            return entity_collider_circle_rect(other, entity);
        } else {
            return entity_collider_rect_rect(entity, other);
        }
    }
}

bool entity_collider_exists(Entity* entity) {
    return entity->collider != NULL;
}