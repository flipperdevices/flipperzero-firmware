#pragma once

#include "expansion_settings_filename.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t uart_index;
} ExpansionSettings;

bool expansion_settings_load(ExpansionSettings* settings);

bool expansion_settings_save(ExpansionSettings* settings);

#ifdef __cplusplus
}
#endif
