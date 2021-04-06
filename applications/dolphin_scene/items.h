#pragma once
#include "dolphin_scene/dolphin_scene.h"

const Item* Home[ITEMS_NUM];
const Item** Scenes[1];

const Item* is_nearby(SceneState* state);
const Item** get_scene(SceneState* state);