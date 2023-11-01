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
