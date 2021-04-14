#pragma once
#include "dolphin_scene.h"

#define ITEMS_NUM 4

const Item* is_nearby(SceneState* state);
const Item** get_scene(SceneState* state);
const void activate_item_callback(SceneState* state, Canvas* canvas);
