#pragma once
#include "dolphin/scenes/scene.h"

typedef enum {
    ItemsPC,
    ItemsEnumTotal,
} ItemsEnum;

const Vec2 item_get_pos(SceneState* state, ItemsEnum item);
const Item* is_nearby(SceneState* state);
const Item** get_scene(SceneState* state);
const void scene_activate_item_callback(SceneState* state, Canvas* canvas);
