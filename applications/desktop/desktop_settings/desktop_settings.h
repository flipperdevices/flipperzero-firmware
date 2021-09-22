#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t favorite;
} DesktopSettings;

bool desktop_settings_load(DesktopSettings* desktop_settings);

bool desktop_settings_save(DesktopSettings* desktop_settings);
