#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XP_MODE_BAR 0
#define XP_MODE_PERCENT 1
#define XP_MODE_INVERTED_PERCENT 2
#define XP_MODE_RETRO_3 3
#define XP_MODE_RETRO_5 4
#define XP_MODE_BAR_PERCENT 5
#define XP_MODE_NONE 6

typedef struct {
    bool background;
    bool image;
    bool name;
    bool mood;
    bool level;
    bool xp_text;
    uint8_t xp_mode;
    bool multipage;
} PassportSettings;

bool passport_settings_load(PassportSettings* passport_settings);

bool passport_settings_save(PassportSettings* passport_settings);

#ifdef __cplusplus
}
#endif