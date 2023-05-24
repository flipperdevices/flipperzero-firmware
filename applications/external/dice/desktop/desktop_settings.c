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
    bool ret = saved_struct_load(
        DESKTOP_SETTINGS_PATH,
        x,
        sizeof(DesktopSettings),
        DESKTOP_SETTINGS_MAGIC,
        DESKTOP_SETTINGS_VER);

    if(!ret) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        storage_common_copy(storage, DESKTOP_SETTINGS_OLD_PATH, DESKTOP_SETTINGS_PATH);
        storage_common_remove(storage, DESKTOP_SETTINGS_OLD_PATH);
        furi_record_close(RECORD_STORAGE);
        ret = saved_struct_load(
            DESKTOP_SETTINGS_PATH,
            x,
            sizeof(DesktopSettings),
            DESKTOP_SETTINGS_MAGIC,
            DESKTOP_SETTINGS_VER);
    }

    return ret;
}