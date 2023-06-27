#pragma once

#include <furi.h>

typedef struct {
    const char* name;
    const char* appid;
} DesktopSettingsApplication;

extern const DesktopSettingsApplication FLIPPER_APPS2[];
extern const size_t FLIPPER_APPS2_COUNT;