#pragma once

#include <lib/subghz/devices/devices.h>
#include <applications/main/subghz/helpers/subghz_types.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include "subghz/helpers/subghz_types.h"

const SubGhzDevice* subbrute_radio_device_loader_set(
    const SubGhzDevice* current_radio_device,
    SubGhzRadioDeviceType radio_device_type);

void subbrute_radio_device_loader_end(const SubGhzDevice* radio_device);
