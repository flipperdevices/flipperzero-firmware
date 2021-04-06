#pragma once
#include "dolphin_scene/items.h"

void smash_tv(Canvas* canvas, void* model);
void draw_tv(Canvas* canvas, void* model);
void sofa_sit(Canvas* canvas, void* model);
void inspect_painting(Canvas* canvas, void* model);
void pc_callback(Canvas* canvas, void* model);

const Item* Home[ITEMS_NUM];
const Item** Scenes[1];
const Item* is_nearby(SceneState* state);
