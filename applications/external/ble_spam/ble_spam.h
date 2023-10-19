#pragma once

#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>

#include "scenes/_setup.h"

enum {
    ViewMain,
    ViewVariableItemList,
};

typedef struct Attack Attack;

typedef struct {
    Attack* attack;

    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    VariableItemList* variable_item_list;
} Ctx;
