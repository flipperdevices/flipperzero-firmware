#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SubGhzDevices SubGhzDevices;

const SubGhzDevice* subghz_devices_get_by_name(const char* device_name);
bool subghz_devices_begin(const SubGhzDevice* device);
void subghz_devices_end(const SubGhzDevice* device);
void subghz_devices_reset(const SubGhzDevice* device);
void subghz_devices_sleep(const SubGhzDevice* device);
void subghz_devices_idle(const SubGhzDevice* device);
void subghz_devices_load_preset(const SubGhzDevice* device, FuriHalSubGhzPreset preset);
uint32_t subghz_devices_set_frequency(const SubGhzDevice* device, uint32_t frequency);
void subghz_devices_set_async_mirror_pin(const SubGhzDevice* device, const GpioPin* gpio);
bool subghz_devices_set_tx(const SubGhzDevice* device);
bool subghz_devices_start_async_tx(const SubGhzDevice* device, void* callback, void* context);
bool subghz_devices_is_async_complete_tx(const SubGhzDevice* device);
void subghz_devices_stop_async_tx(const SubGhzDevice* device);
void subghz_devices_set_rx(const SubGhzDevice* device);
void subghz_devices_start_async_rx(const SubGhzDevice* device, void* callback, void* context);
void subghz_devices_stop_async_rx(const SubGhzDevice* device);

#ifdef __cplusplus
}
#endif
