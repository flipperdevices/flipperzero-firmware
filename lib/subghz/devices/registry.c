#include "registry.h"

const SubGhzDevice* subghz_device_registry_get_by_name(
    const SubGhzDeviceRegistry* subghz_device,
    const char* name) {
    furi_assert(subghz_device);

    if(name != NULL) {
        for(size_t i = 0; i < subghz_device_registry_count(subghz_device); i++) {
            if(strcmp(name, subghz_device->items[i]->name) == 0) {
                return subghz_device->items[i];
            }
        }
    }
    return NULL;
}

const SubGhzDevice*
    subghz_device_registry_get_by_index(const SubGhzDeviceRegistry* subghz_device, size_t index) {
    furi_assert(subghz_device);
    if(index < subghz_device_registry_count(subghz_device)) {
        return subghz_device->items[index];
    } else {
        return NULL;
    }
}

size_t subghz_device_registry_count(const SubGhzDeviceRegistry* subghz_device) {
    furi_assert(subghz_device);
    return subghz_device->size;
}
