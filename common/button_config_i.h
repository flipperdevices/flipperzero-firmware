#pragma once

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>

#include "button_config.h"
#include "config_colors.h"
#include "config_keystroke.h"
#include "config_tones.h"
#include "flipboard_model.h"
#include "keystroke_selector.h"
#include "menu_callback.h"

struct ButtonConfig {
    FlipboardModel* model;
    ViewDispatcher* view_dispatcher;

    // menu_buttons is used for showing a list of buttons to be configured.
    Submenu* menu_buttons;
    uint32_t view_menu_buttons_id;

    // item_list is used for showing the configurations of a button.
    VariableItemList* item_list;
    uint32_t view_item_list_id;

    // text_input is used for entering a Message.
    TextInput* text_input;
    uint32_t view_text_input_id;

    // keystroke_selector is used for selecting a keystroke.
    KeystrokeSelector* keystroke_selector;
    uint32_t view_keystroke_selector_id;
};

typedef struct {
    void* app;
    uint8_t key;
    uint8_t index;
} VariableItemContext;