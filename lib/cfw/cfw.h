#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <furi_hal_serial_types.h>
#include <toolbox/colors.h>
// #include <gui/icon_i.h>
// #include <power/power_service/power.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CFW_SETTINGS_PATH CFG_PATH("cfw_settings.txt")
#define CFW_MENU_PATH CFG_PATH("cfw_mainmenu.txt")
#define CFW_MENU_GAMESMODE_PATH CFG_PATH("cfw_gamesmenu.txt")
#define CFW_APPS_PATH CFW_MENU_PATH
#define NAMESPOOF_HEADER "Flipper Name File"
#define NAMESPOOF_VERSION 1
#define NAMESPOOF_PATH EXT_PATH("dolphin/name.txt")

typedef enum {
    MenuStyleList,
    MenuStyleWii,
    MenuStyleDsi,
    MenuStylePs4,
    MenuStyleVertical,
    MenuStyleC64,
    MenuStyleEurocorp,
    MenuStyleCompact,
    MenuStyleTerminal,
    MenuStyleCount,
} MenuStyle;

typedef enum {
    SpiDefault, // cs on pa4
    SpiExtra, // cs on pc3
    SpiCount,
} SpiHandle;

typedef enum {
    VgmColorModeDefault,
    VgmColorModeCustom,
    VgmColorModeRgbBacklight,
    VgmColorModeCount,
} VgmColorMode;

typedef struct {
    char* manifest_name;
    MenuStyle menu_style;
    MenuStyle game_menu_style;
    uint32_t start_point;
    uint32_t game_start_point;
    bool lock_menu_type;
    bool sort_dirs_first;
    bool dark_mode;
    uint32_t charge_cap;
    uint32_t favorite_timeout;
    SpiHandle spi_cc1101_handle;
    SpiHandle spi_nrf24_handle;
    FuriHalSerialId uart_esp_channel;
    FuriHalSerialId uart_nmea_channel;
    VgmColorMode vgm_color_mode;
    Rgb565Color vgm_color_fg;
    Rgb565Color vgm_color_bg;
    bool rgb_backlight;
    uint32_t lcd_style;
} CfwSettings;

void CFW_SETTINGS_SAVE();
extern CfwSettings cfw_settings;

#ifdef __cplusplus
}
#endif
