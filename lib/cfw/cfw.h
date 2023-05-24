#pragma once

#include <gui/icon_i.h>
#include <power/power_service/power.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CFW_SETTINGS_PATH CFG_PATH("cfw_settings.txt")
#define CFW_APPS_PATH CFG_PATH("cfw_apps.txt")
#define NAMESPOOF_HEADER "Flipper Name File"
#define NAMESPOOF_VERSION 1
#define NAMESPOOF_PATH EXT_PATH("dolphin/name.txt")

typedef struct {
    bool wii_menu;
    bool bad_pins_format;
    // bool lockscreen_time;
    // bool lockscreen_seconds;
    // bool lockscreen_date;
    // bool lockscreen_statusbar;
    // bool lockscreen_prompt;
    bool sort_dirs_first;
    bool dark_mode;
    // uint32_t favorite_timeout;
    bool rgb_backlight;
} CfwSettings;

void CFW_SETTINGS_SAVE();
CfwSettings* CFW_SETTINGS();

#ifdef __cplusplus
}
#endif
