#include "entity.h"
#include "entity_i.h"
#include <stdlib.h>
#include <furi.h>
#include <math.h>

#ifdef ENTITY_DEBUG
#define ENTITY_D(...) FURI_LOG_D("Entity", __VA_ARGS__)
#else
#define ENTITY_D(...)
#endif

static int32_t entities_count = 0;

int32_t entities_get_count(void) {
    return entities_count;
}

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
    entity->collider_offset = VECTOR_ZERO;
    ENTITY_D("Allocated at %p", entity);
    entity->collider_dirty = false;
    return entity;
}

void entity_collider_add_circle(Entity* entity, float radius) {
    furi_check(entity->collider == NULL, "Collider already added");
    entity->collider = malloc(sizeof(Collider));
    entity->collider->type = ColliderTypeCircle;
    entity->collider->circle.radius = radius;
    entity->collider_dirty = true;
}

void entity_collider_add_rect(Entity* entity, float width, float height) {
    furi_check(entity->collider == NULL, "Collider already added");
    entity->collider = malloc(sizeof(Collider));
    entity->collider->type = ColliderTypeRect;
    entity->collider->rect.half_width = width / 2;
    entity->collider->rect.half_height = height / 2;
    entity->collider_dirty = true;
}

void entity_collider_remove(Entity* entity) {
    furi_check(entity->collider != NULL, "Collider not added");
    free(entity->collider);
    entity->collider = NULL;
    entity->collider_dirty = false;
}

void entity_collider_offset_set(Entity* entity, Vector offset) {
    entity->collider_offset = offset;
    entity->collider_dirty = true;
}

Vector entity_collider_offset_get(Entity* entity) {
    return entity->collider_offset;
}

static Vector entity_collider_position_get(Entity* entity) {
    return (Vector){
        .x = entity->position.x + entity->collider_offset.x,
        .y = entity->position.y + entity->collider_offset.y,
    };
}

void entity_free(Entity* entity) {
    entities_count--;
    ENTITY_D("Freeing at %p", entity);
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
    entity->collider_dirty = true;
}

void* entity_context_get(Entity* entity) {
    return entity->context;
}

void entity_call_start(Entity* entity, GameManager* manager) {
    if(entity->description && entity->description->start) {
        entity->description->start(entity, manager, entity->context);
    }
}

void entity_call_stop(Entity* entity, GameManager* manager) {
    if(entity->description && entity->description->stop) {
        entity->description->stop(entity, manager, entity->context);
    }
}

void entity_call_update(Entity* entity, GameManager* manager) {
    if(entity->description && entity->description->update) {
        entity->description->update(entity, manager, entity->context);
    }
}

void entity_call_render(Entity* entity, GameManager* manager, Canvas* canvas) {
    if(entity->description && entity->description->render) {
        entity->description->render(entity, manager, canvas, entity->context);
    }
}

void entity_call_collision(Entity* entity, Entity* other, GameManager* manager) {
    if(entity->description && entity->description->collision) {
        entity->description->collision(entity, other, manager, entity->context);
    }
}

bool entity_collider_circle_circle(Entity* entity, Entity* other) {
    Vector pos1 = entity_collider_position_get(entity);
    Vector pos2 = entity_collider_position_get(other);

    Vector delta = vector_sub(pos1, pos2);
    return vector_length(delta) < entity->collider->circle.radius + other->collider->circle.radius;
}

bool entity_collider_rect_rect(Entity* entity, Entity* other) {
    Vector pos1 = entity_collider_position_get(entity);
    Vector pos2 = entity_collider_position_get(other);

    float left1 = pos1.x - entity->collider->rect.half_width;
    float right1 = pos1.x + entity->collider->rect.half_width;
    float top1 = pos1.y - entity->collider->rect.half_height;
    float bottom1 = pos1.y + entity->collider->rect.half_height;

    float left2 = pos2.x - other->collider->rect.half_width;
    float right2 = pos2.x + other->collider->rect.half_width;
    float top2 = pos2.y - other->collider->rect.half_height;
    float bottom2 = pos2.y + other->collider->rect.half_height;

    return left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2;
}

bool entity_collider_circle_rect(Entity* circle, Entity* rect) {
    Vector pos1 = entity_collider_position_get(circle);
    Vector pos2 = entity_collider_position_get(rect);

    float left = pos2.x - rect->collider->rect.half_width;
    float right = pos2.x + rect->collider->rect.half_width;
    float top = pos2.y - rect->collider->rect.half_height;
    float bottom = pos2.y + rect->collider->rect.half_height;

    float closestX = fmaxf(left, fminf(pos1.x, right));
    float closestY = fmaxf(top, fminf(pos1.y, bottom));

    float dx = pos1.x - closestX;
    float dy = pos1.y - closestY;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < circle->collider->circle.radius;
}

bool entity_collider_check_collision(Entity* entity, Entity* other) {
    furi_check(entity->collider);
    furi_check(other->collider);

    if(entity->collider->type == ColliderTypeCircle) {
        Entity* circle = entity;
        if(other->collider->type == ColliderTypeCircle) {
            return entity_collider_circle_circle(circle, other);
        } else {
            return entity_collider_circle_rect(circle, other);
        }
    } else {
        Entity* rect = entity;
        if(other->collider->type == ColliderTypeCircle) {
            return entity_collider_circle_rect(other, rect);
        } else {
            return entity_collider_rect_rect(rect, other);
        }
    }
}

bool entity_collider_exists(Entity* entity) {
    return entity->collider != NULL;
}

void entity_send_event(
    Entity* sender,
    Entity* receiver,
    GameManager* manager,
    uint32_t type,
    EntityEventValue value) {
    if(receiver->description && receiver->description->event) {
        EntityEvent event = {
            .type = type,
            .sender = sender,
            .value = value,
        };
        receiver->description->event(receiver, manager, event, receiver->context);
    }
}