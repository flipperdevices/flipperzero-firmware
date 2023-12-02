#pragma once

#include <furi.h>

typedef struct Resources Resources;

typedef enum {
    ResourceIdSubGhzSignal,
    ResourceIdLedDriver,
    ResourceIdInfrared,
} ResourceId;

/**
 * @brief Allocates a new Resources object for access to Flipper hardware.
 * @return A pointer to the allocated Resources struct.
*/
Resources* resources_alloc();

/**
 * @brief Acquires a resource.
 * @details This method acquires a resource.  If the resource is already acquired,
 * this method will block until the resource is released.
 * @param resources The resources struct to use for hardware access.
*/
bool resources_acquire(Resources* resources, ResourceId id, uint32_t timeout);

/**
 * @brief Releases a resource.
 * @details This method releases a resource.
 * @param resources The resources struct to use for hardware access.
*/
bool resources_release(Resources* resources, ResourceId id);

/**
 * @brief Frees a Resources struct.
 * @param resources The Resources struct to free.
*/
void resources_free(Resources* resources);