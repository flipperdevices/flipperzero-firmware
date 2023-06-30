#include "cfw.h"
#include "private.h"
#include <furi_hal.h>
#include <flipper_format/flipper_format.h>

#define TAG "CfwSettings"

CfwSettings cfw_settings = {
    .wii_menu = false, // OFF
    .start_point = 0, // First Item
    .lock_menu_type = true, // Adv Grid VS FALSE=LIST
    .sort_dirs_first = true, // ON
    .dark_mode = false, // OFF
    .charge_cap = 100, // 100%
    .rgb_backlight = false, // OFF
};

void CFW_SETTINGS_LOAD() {
    if(!furi_hal_is_normal_boot()) return;

    CfwSettings* x = &cfw_settings;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, CFW_SETTINGS_PATH)) {
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "wii_menu", &x->wii_menu, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "start_point", &x->start_point, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "lock_menu_type", &x->lock_menu_type, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "dark_mode", &x->dark_mode, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "charge_cap", &x->charge_cap, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
    FURI_LOG_I(TAG, "RM WUZ HERE");
}

void CFW_SETTINGS_SAVE() {
    if(!furi_hal_is_normal_boot()) return;

    CfwSettings* x = &cfw_settings;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_always(file, CFW_SETTINGS_PATH)) {
        flipper_format_write_bool(file, "wii_menu", &x->wii_menu, 1);
        flipper_format_write_uint32(file, "start_point", &x->start_point, 1);
        flipper_format_write_bool(file, "lock_menu_type", &x->lock_menu_type, 1);
        flipper_format_write_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_write_bool(file, "dark_mode", &x->dark_mode, 1);
        flipper_format_write_uint32(file, "charge_cap", &x->charge_cap, 1);
        flipper_format_write_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

CfwSettings* CFW_SETTINGS() {
    return &cfw_settings;
}
