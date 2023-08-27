#pragma once

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include "subbrute_protocols.h"

typedef struct {
    uint8_t repeat_values[SubBruteAttackTotalCount];
    uint32_t last_index;
} SubBruteSettings;

SubBruteSettings* subbrute_settings_alloc(void);
void subbrute_settings_free(SubBruteSettings* instance);
void subbrute_settings_load(SubBruteSettings* instance);
bool subbrute_settings_save(SubBruteSettings* instance);
void subbrute_settings_set_value(SubBruteSettings* instance, SubBruteAttacks index, uint8_t value);
uint8_t subbrute_settings_get_value(SubBruteSettings* instance, SubBruteAttacks index);
void subbrute_settings_set_repeats(SubBruteSettings* instance, const uint8_t* repeated_values);
uint8_t subbrute_settings_get_current_repeats(SubBruteSettings* instance);
