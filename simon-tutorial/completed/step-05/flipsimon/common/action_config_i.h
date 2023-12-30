#pragma once

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>

#include "../app_config.h"

#include "action_config.h"
#define DEFINE_COLOR_NAMES_AND_VALUES "action_config_i.h"
#include "config_colors.h"
#include "config_keystroke.h"
#include "config_tones.h"
#include "flipboard_model.h"
#include "keystroke_selector.h"
#include "menu_callback.h"

struct ActionConfig {
    FlipboardModel* model;
    ViewDispatcher* view_dispatcher;

    // menu_actionss is used for showing a list of action to be configured.
    Submenu* menu_actions;
    uint32_t view_menu_actions_id;

    // item_list is used for showing the configurations of an action.
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

static void populate_variable_item_list(ActionConfig* button_config, ActionModel* bm);