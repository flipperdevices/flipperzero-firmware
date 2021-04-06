#pragma once

#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

#include <gui/gui.h>
#include "dolphin_scene/dolphin_scene.h"

void smash_tv(Canvas* canvas, void* model);
void draw_tv(Canvas* canvas, void* model);
void sofa_sit(Canvas* canvas, void* model);
void inspect_painting(Canvas* canvas, void* model);
void pc_callback(Canvas* canvas, void* model);

static const Item TV = {
    .layer = 7,
    .timeout = 10,
    .x = 160,
    .y = 34,
    .icon = I_TV_20x24,
    .action_name = "Use",
    .draw = NULL,
    .callback = smash_tv};

static const Item Painting = {
    .layer = 3,
    .timeout = 20,
    .x = 160,
    .y = 10,
    .icon = I_Home_painting_17x20,
    .action_name = "Inspect",
    .draw = NULL,
    .callback = inspect_painting};

static const Item Sofa = {
    .layer = 4,
    .timeout = 100,
    .x = 250,
    .y = 34,
    .icon = I_Sofa_40x13,
    .action_name = "Sit",
    .draw = NULL,
    .callback = sofa_sit};

static const Item PC = {
    .layer = 4,
    .timeout = 100,
    .x = 400,
    .y = 10,
    .icon = I_PC_22x29,
    .action_name = "Use",
    .draw = NULL,
    .callback = pc_callback};

const Item* Home[ITEMS_NUM];
const Item** Scenes[1];

const Item* is_nearby(SceneState* state);

#endif
