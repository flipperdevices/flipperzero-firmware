
#pragma once

#include <math.h>
#include <furi.h>
#include <furi_hal.h>
#include <lib/flipper_format/flipper_format.h>

typedef struct SubGhzSetting SubGhzSetting;

SubGhzSetting* subghz_setting_alloc(void);
void subghz_setting_free(SubGhzSetting* instance);
void subghz_setting_load(SubGhzSetting* instance);
size_t subghz_setting_get_frequency_count(SubGhzSetting* instance);
size_t subghz_setting_get_hopper_frequency_count(SubGhzSetting* instance);
uint32_t subghz_setting_get_frequency(SubGhzSetting* instance, size_t idx);
uint32_t subghz_setting_get_hopper_frequency(SubGhzSetting* instance, size_t idx);
uint32_t subghz_setting_get_frequency_default_index(SubGhzSetting* instance);
