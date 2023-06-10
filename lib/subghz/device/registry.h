#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SubGhzDeviceRegistry SubGhzDeviceRegistry;
typedef struct SubGhzDevice SubGhzDevice;

struct SubGhzDeviceRegistry {
    const SubGhzDevice** items;
    const size_t size;
};

/**
 * Registration by name SubGhzDevice.
 * @param subghz_device SubGhzDeviceRegistry
 * @param name SubGhzDevice name
 * @return SubGhzDevice* pointer to a SubGhzDevice instance
 */
const SubGhzDevice*
    subghz_device_registry_get_by_name(const SubGhzDeviceRegistry* subghz_device, const char* name);

/**
 * Registration subghzdevice by index in array SubGhzDevice.
 * @param subghz_device SubGhzDeviceRegistry
 * @param index SubGhzDevice by index in array
 * @return SubGhzDevice* pointer to a SubGhzDevice instance
 */
const SubGhzDevice*
    subghz_device_registry_get_by_index(const SubGhzDeviceRegistry* subghz_device, size_t index);

/**
 * Getting the number of registered subghzdevices.
 * @param subghz_device SubGhzDeviceRegistry
 * @return Number of subghzdevices
 */
size_t subghz_device_registry_count(const SubGhzDeviceRegistry* subghz_device);

#ifdef __cplusplus
}
#endif
