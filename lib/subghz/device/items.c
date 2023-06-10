#include "items.h"

const SubGhzDevice* subghz_device_registry_items[] = {
    &subghz_device_cc1101_ext,

};

const SubGhzDeviceRegistry subghz_device_registry = {
    .items = subghz_device_registry_items,
    .size = COUNT_OF(subghz_device_registry_items)};
