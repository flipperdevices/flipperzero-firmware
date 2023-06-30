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
    uint32_t start_point;
    bool lock_menu_type;
    bool sort_dirs_first;
    bool dark_mode;
    uint32_t charge_cap;
    bool rgb_backlight;
} CfwSettings;

void CFW_SETTINGS_SAVE();
CfwSettings* CFW_SETTINGS();

#ifdef __cplusplus
}
#endif
