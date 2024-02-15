#include "desktop_settings.h"

bool DESKTOP_SETTINGS_SAVE(DesktopSettings* x) {
    return saved_struct_save(
        DESKTOP_SETTINGS_PATH,
        x,
        sizeof(DesktopSettings),
        DESKTOP_SETTINGS_MAGIC,
        DESKTOP_SETTINGS_VER);
}

bool DESKTOP_SETTINGS_LOAD(DesktopSettings* x) {
    return saved_struct_load(
        DESKTOP_SETTINGS_PATH,
        x,
        sizeof(DesktopSettings),
        DESKTOP_SETTINGS_MAGIC,
        DESKTOP_SETTINGS_VER);
}