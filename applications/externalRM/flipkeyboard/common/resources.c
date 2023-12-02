#include <furi.h>
#include "resources.h"

struct Resources {
    FuriMutex* ll_tim1;
};

/**
 * @brief Allocates a new Resources object for access to Flipper hardware.
 * @return A pointer to the allocated Resources struct.
*/
Resources* resources_alloc() {
    Resources* resources = (Resources*)malloc(sizeof(Resources));
    resources->ll_tim1 = furi_mutex_alloc(FuriMutexTypeNormal);
    return resources;
}

/**
 * @brief Acquires a resource.
 * @details This method acquires a resource.  If the resource is already acquired,
 * this method will block until the resource is released.
 * @param resources The resources struct to use for hardware access.
*/
bool resources_acquire(Resources* resources, ResourceId id, uint32_t timeout) {
    UNUSED(id);
    if(!resources) {
        return false;
    }
    return furi_mutex_acquire(resources->ll_tim1, timeout) == FuriStatusOk;
}

/**
 * @brief Releases a resource.
 * @details This method releases a resource.
 * @param resources The resources struct to use for hardware access.
*/
bool resources_release(Resources* resources, ResourceId id) {
    UNUSED(id);
    if(!resources) {
        return false;
    }
    return furi_mutex_release(resources->ll_tim1) == FuriStatusOk;
}

/**
 * @brief Frees a Resources struct.
 * @param resources The Resources struct to free.
*/
void resources_free(Resources* resources) {
    if(resources) {
        furi_mutex_free(resources->ll_tim1);
        free(resources);
    }
}