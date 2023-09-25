#pragma once

#include "key_config.h"

#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

struct KeyConfig {
    FlipboardModel* model;
    ViewDispatcher* view_dispatcher;
    Submenu* menu_keys;
    uint32_t view_menu_keys_id;
    VariableItemList* item_list;
    uint32_t view_item_list_id;
    TextInput* text_input;
    uint32_t view_text_input_id;
};
