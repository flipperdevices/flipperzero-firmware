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

/**
 * @brief Remove all entities from the level
 * 
 * @param level level instance
 */
void level_clear(Level* level);

/**
 * @brief Add an entity to the level
 * 
 * @param level level instance
 * @param behaviour entity behaviour
 * @return Entity* 
 */
Entity* level_add_entity(Level* level, const EntityDescription* behaviour);

/**
 * @brief Remove an entity from the level
 * 
 * @param level level instance
 * @param entity entity to remove
 */
void level_remove_entity(Level* level, Entity* entity);

/**
 * @brief Send an event to all entities of a certain type in the level
 * 
 * @param level level instance
 * @param sender entity that sends the event
 * @param receiver_desc entity description that will receive the event, NULL for all entities
 * @param type event type
 * @param value event value
 */
void level_send_event(
    Level* level,
    Entity* sender,
    const EntityDescription* receiver_desc,
    uint32_t type,
    EntityEventValue value);

/**
 * @brief Get the count of entities of a certain type in the level, or all entities if description is NULL
 * 
 * @param level level instance
 * @param description entity description, NULL for all entities
 * @return size_t entity count 
 */
size_t level_entity_count(const Level* level, const EntityDescription* description);

/**
 * @brief Get the context of the level
 * 
 * @param level level instance
 * @return void* context
 */
void* level_context_get(Level* level);

#ifdef __cplusplus
}
#endif