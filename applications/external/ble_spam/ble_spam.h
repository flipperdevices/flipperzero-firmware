#pragma once

#include <gui/view_dispatcher.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>

#include "scenes/_setup.h"

enum {
    ViewMain,
    ViewByteInput,
    ViewSubmenu,
    ViewTextInput,
    ViewVariableItemList,
};

typedef struct Attack Attack;

typedef struct {
    Attack* attack;
    uint8_t byte_store[3];

    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    ByteInput* byte_input;
    Submenu* submenu;
    TextInput* text_input;
    VariableItemList* variable_item_list;
} Ctx;
