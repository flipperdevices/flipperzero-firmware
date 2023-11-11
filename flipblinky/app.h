#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include "../common/app_menu.h"
#include "../common/backlight.h"
#include "../common/flipboard.h"
#include "../common/flipboard_file.h"
#include "../common/flipboard_model.h"
#include "../common/flipboard_model_ref.h"
#include "../common/keyboard.h"
#include "../common/key_config.h"
#include "../common/key_monitor.h"
#include "../common/leds.h"
#include "../common/speaker.h"

typedef struct FlipboardBlinkyModel {
    FuriTimer* timer;
    uint32_t period_ms;
    uint32_t colors[6];
    uint8_t effect_counter;
    uint8_t effect_id;
    uint8_t max_effect_id;
    uint32_t show_details_counter;
} FlipboardBlinkyModel;

enum LedColors {
    LedColorBlack = 0x000000,
    LedColorRed = 0xFF0000,
    LedColorOrange = 0xFF1F00,
    LedColorYellow = 0xFF7F00,
    LedColorGreen = 0x00FF00,
    LedColorCyan = 0x00FFFF,
    LedColorBlue = 0x0000FF,
    LedColorViolet = 0x1F00FF,
    LedColorMagenta = 0x7F00FF,
    LedColorWhite = 0xFFFFFF,
};