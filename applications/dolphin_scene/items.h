#pragma once
#include <gui/gui.h>
#include "dolphin_scene/dolphin_scene.h"

void smash_tv(Canvas* canvas, void* model);
void draw_tv(Canvas* canvas, void* model);
void sofa_sit(Canvas* canvas, void* model);
void inspect_painting(Canvas* canvas, void* model);
void pc_callback(Canvas* canvas, void* model);

static const Item TV = {
    .layer = 7,
    .x = 240,
    .y = 34,
    .icon = I_TV_20x24,
    .action_name = "Use",
    .draw = NULL,
    .callback = smash_tv};

static const Item Painting = {
    .layer = 3,
    .x = 265,
    .y = 10,
    .icon = I_Home_painting_17x20,
    .action_name = "Inspect",
    .draw = NULL,
    .callback = inspect_painting};

static const Item Sofa = {
    .layer = 4,
    .x = 350,
    .y = 34,
    .icon = I_Sofa_40x13,
    .action_name = "Sit",
    .draw = NULL,
    .callback = sofa_sit};

static const Item PC = {
    .layer = 4,
    .x = 500,
    .y = 10,
    .icon = I_PC_22x29,
    .action_name = "Use",
    .draw = NULL,
    .callback = pc_callback};

static const Item* Home[] = {&TV, &Sofa, &Painting, &PC};
static const Item** Scenes[] = {*&Home};
const Item* is_nearby(SceneState* state);
