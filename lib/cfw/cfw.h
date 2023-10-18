#pragma once

#include <stdint.h>
#include <stdbool.h>
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
    AnimStyleNone,
    AnimStyleDefault,
    AnimStyleMinimal,
    AnimStyle420,
    AnimStyle42018,
    AnimStyleALL,
    AnimStyleAnime,
    AnimStyleAnime420,
    AnimStyleAnime42018,
    AnimStyleBMO,
    AnimStyleCherry,
    AnimStyleCorpLogos,
    AnimStyleDBZ,
    AnimStyleDigim0n,
    AnimStyleDolphin,
    AnimStyleHackz,
    AnimStyleMario,
    AnimStyleNYAN,
    AnimStyleOnePiece,
    AnimStyleP0kemon,
    AnimStyleRM18,
    AnimStyleRMSelect,
    AnimStyleRMSelect18,
    AnimStyleSAO,
    AnimStyleScience,
    AnimStyleSJUMP,
    AnimStyleSquatch,
    AnimStyleStock,
    AnimStyleWatchDogs,
    AnimStyleCount,
} AnimStyle;

typedef enum {
    SpiDefault, // cs on pa4
    SpiExtra, // cs on pc3
    SpiCount,
} SpiHandle;

typedef enum {
    UARTDefault, // pin 13,14
    UARTExtra, // pin 15,16
    UARTCount,
} UARTChannel;

typedef struct {
    AnimStyle anim_style;
    MenuStyle menu_style;
    MenuStyle game_menu_style;
    uint32_t start_point;
    uint32_t game_start_point;
    bool lock_menu_type;
    bool sort_dirs_first;
    bool dark_mode;
    uint32_t charge_cap;
    SpiHandle spi_cc1101_handle;
    SpiHandle spi_nrf24_handle;
    UARTChannel uart_esp_channel;
    UARTChannel uart_nmea_channel;
    UARTChannel uart_general_channel;
    bool rgb_backlight;
    uint32_t lcd_style;
} CfwSettings;

void CFW_SETTINGS_SAVE();
CfwSettings* CFW_SETTINGS();

#ifdef __cplusplus
}
#endif
