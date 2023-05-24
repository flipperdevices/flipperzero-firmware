#include "cfw.h"
#include "private.h"
#include <furi_hal.h>
#include <flipper_format/flipper_format.h>

#define TAG "CfwSettings"

CfwSettings cfw_settings = {
    .wii_menu = false, // OFF
    .bad_pins_format = false, // OFF
    // .lockscreen_time = true, // ON
    // .lockscreen_seconds = false, // OFF
    // .lockscreen_date = true, // ON
    // .lockscreen_statusbar = true, // ON
    // .lockscreen_prompt = true, // ON
    .sort_dirs_first = true, // ON
    .dark_mode = false, // OFF
    // .favorite_timeout = 0, // OFF
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
        flipper_format_read_bool(file, "bad_pins_format", &x->bad_pins_format, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_bool(file, "lockscreen_time", &x->lockscreen_time, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_bool(file, "lockscreen_seconds", &x->lockscreen_seconds, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_bool(file, "lockscreen_date", &x->lockscreen_date, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_bool(file, "lockscreen_statusbar", &x->lockscreen_statusbar, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_bool(file, "lockscreen_prompt", &x->lockscreen_prompt, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "dark_mode", &x->dark_mode, 1);
        // flipper_format_rewind(file);
        // flipper_format_read_uint32(file, "favorite_timeout", &x->favorite_timeout, 1);
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
        flipper_format_write_bool(file, "bad_pins_format", &x->bad_pins_format, 1);
        // flipper_format_write_bool(file, "lockscreen_time", &x->lockscreen_time, 1);
        // flipper_format_write_bool(file, "lockscreen_seconds", &x->lockscreen_seconds, 1);
        // flipper_format_write_bool(file, "lockscreen_date", &x->lockscreen_date, 1);
        // flipper_format_write_bool(file, "lockscreen_statusbar", &x->lockscreen_statusbar, 1);
        // flipper_format_write_bool(file, "lockscreen_prompt", &x->lockscreen_prompt, 1);
        flipper_format_write_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_write_bool(file, "dark_mode", &x->dark_mode, 1);
        // flipper_format_write_uint32(file, "favorite_timeout", &x->favorite_timeout, 1);
        flipper_format_write_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

CfwSettings* CFW_SETTINGS() {
    return &cfw_settings;
}
