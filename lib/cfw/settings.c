#include "cfw.h"
#include "private.h"
#include <furi_hal.h>
#include <rgb_backlight.h>
#include <flipper_format/flipper_format.h>

#define TAG "CfwSettings"

CfwSettings cfw_settings = {
    .manifest_name = "manifest.txt", // File Name of Manifest to use.
    .menu_style = MenuStyleList, // List
    .game_menu_style = MenuStyleWii, // List
    .start_point = 0, // First Item
    .game_start_point = 0, // First Item
    .lock_menu_type = true, // Adv Grid VS FALSE=LIST
    .sort_dirs_first = true, // ON
    .show_hidden_files = true, // ON
    .show_internal_tab = true, // ON
    .favorite_timeout = 0, // OFF
    .dark_mode = false, // OFF
    .charge_cap = 100, // 100%
    .spi_cc1101_handle = SpiDefault, // &furi_hal_spi_bus_handle_external
    .spi_nrf24_handle = SpiDefault, // &furi_hal_spi_bus_handle_external
    .uart_esp_channel = FuriHalSerialIdUsart, // pin 13,14
    .uart_nmea_channel = FuriHalSerialIdUsart, // pin 13,14
    .rgb_backlight = false, // OFF
    .lcd_style = 0, // Static
    .vgm_color_mode = VgmColorModeDefault, // Default
    .vgm_color_fg.value = 0x0000, // Default Black
    .vgm_color_bg.value = 0xFC00, // Default Orange
    .spoof_color = FuriHalVersionColorUnknown, // Real
};

void cfw_settings_load(void) {
    if(!furi_hal_is_normal_boot()) return;

    CfwSettings* x = &cfw_settings;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    FuriString* manifest_name = furi_string_alloc();

    if(flipper_format_file_open_existing(file, CFW_SETTINGS_PATH)) {
        flipper_format_rewind(file);
        flipper_format_read_string(file, "manifest_name", manifest_name);
        x->manifest_name = strdup(furi_string_get_cstr(manifest_name));
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "menu_style", (uint32_t*)&x->menu_style, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "game_menu_style", (uint32_t*)&x->game_menu_style, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "start_point", &x->start_point, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "game_start_point", &x->game_start_point, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "lock_menu_type", &x->lock_menu_type, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "show_hidden_files", &x->show_hidden_files, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "show_internal_tab", &x->show_internal_tab, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "favorite_timeout", &x->favorite_timeout, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "dark_mode", &x->dark_mode, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "charge_cap", &x->charge_cap, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "spi_cc1101_handle", (uint32_t*)&x->spi_cc1101_handle, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "spi_nrf24_handle", (uint32_t*)&x->spi_nrf24_handle, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "uart_esp_channel", (uint32_t*)&x->uart_esp_channel, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "uart_nmea_channel", (uint32_t*)&x->uart_nmea_channel, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "vgm_color_mode", (uint32_t*)&x->vgm_color_mode, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(
            file, "vgm_color_fg.value", (uint32_t*)&x->vgm_color_fg.value, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(
            file, "vgm_color_bg.value", (uint32_t*)&x->vgm_color_bg.value, 1);
        flipper_format_rewind(file);
        flipper_format_read_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "lcd_style", (uint32_t*)&x->lcd_style, 1);
        flipper_format_rewind(file);
        flipper_format_read_uint32(file, "spoof_color", (uint32_t*)&x->spoof_color, 1);
    }
    furi_string_free(manifest_name);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    rgb_backlight_load_settings(x->rgb_backlight);
    FURI_LOG_I(TAG, "RM WUZ HERE");
}

void cfw_settings_save() {
    if(!furi_hal_is_normal_boot()) return;

    CfwSettings* x = &cfw_settings;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_always(file, CFW_SETTINGS_PATH)) {
        // uint32_t u;
        // uint32_t e;
        // u = x->menu_style;
        // e = CLAMP(u, MenuStyleCount - 1U, 0U);
        FuriString* manifest_name;
        manifest_name = furi_string_alloc_set_str(x->manifest_name);
        flipper_format_write_string(file, "manifest_name", manifest_name);
        flipper_format_write_uint32(file, "menu_style", (uint32_t*)&x->menu_style, 1);
        flipper_format_write_uint32(file, "game_menu_style", (uint32_t*)&x->game_menu_style, 1);
        flipper_format_write_uint32(file, "start_point", &x->start_point, 1);
        flipper_format_write_uint32(file, "game_start_point", &x->game_start_point, 1);
        flipper_format_write_bool(file, "lock_menu_type", &x->lock_menu_type, 1);
        flipper_format_write_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_write_bool(file, "show_hidden_files", &x->show_hidden_files, 1);
        flipper_format_write_bool(file, "show_internal_tab", &x->show_internal_tab, 1);
        flipper_format_write_uint32(file, "favorite_timeout", &x->favorite_timeout, 1);
        flipper_format_write_bool(file, "dark_mode", &x->dark_mode, 1);
        flipper_format_write_uint32(file, "charge_cap", &x->charge_cap, 1);
        flipper_format_write_uint32(
            file, "spi_cc1101_handle", (uint32_t*)&x->spi_cc1101_handle, 1);
        flipper_format_write_uint32(file, "spi_nrf24_handle", (uint32_t*)&x->spi_nrf24_handle, 1);
        flipper_format_write_uint32(file, "uart_esp_channel", (uint32_t*)&x->uart_esp_channel, 1);
        flipper_format_write_uint32(
            file, "uart_nmea_channel", (uint32_t*)&x->uart_nmea_channel, 1);
        flipper_format_write_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
        flipper_format_write_uint32(file, "lcd_style", (uint32_t*)&x->lcd_style, 1);
        flipper_format_write_uint32(file, "spoof_color", (uint32_t*)&x->spoof_color, 1);
        furi_string_free(manifest_name);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}