#pragma once
#include "dolphin_i.h"

/// Items
const Item TV = {
    .layer = 7,
    .x = 40,
    .y = 34,
    .icon = I_TV_20x24,
    .action_name = "Use",
    .draw = draw_tv,
    .callback = smash_tv};

const Item Painting = {
    .layer = 3,
    .x = 65,
    .y = 10,
    .icon = I_Home_painting_17x20,
    .action_name = "Inspect",
    .draw = NULL,
    .callback = inspect_painting};

const Item Sofa = {
    .layer = 5,
    .x = 100,
    .y = 34,
    .icon = I_Sofa_40x13,
    .action_name = "Sit",
    .draw = NULL,
    .callback = sofa_sit};

const Item PC = {
    .layer = 5,
    .x = 200,
    .y = 10,
    .icon = I_PC_22x29,
    .action_name = "Use",
    .draw = NULL,
    .callback = pc_callback};

const Item* Home[] = {&TV, &Sofa, &Painting, &PC};
const Item** Scenes[] = {*&Home};

// Models

typedef struct {
    IconName main;
    IconName back;
} DolphinGfx;

static const DolphinGfx action_gfx[] = {
    [SLEEP] = {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    [IDLE] = {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    [WALK_L] = {.main = A_MDWL_32x32, .back = A_MDWLB_32x32},
    [WALK_R] = {.main = A_MDWR_32x32, .back = A_MDWRB_32x32},
    [EMOTE] = {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    [USE] = {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    [MINDCONTROL] = {.main = A_MDI_32x32, .back = A_MDIB_32x32},
};

const Item* is_nearby(void* model) {
    DolphinViewMainModel* m = model;
    uint8_t item = 0;
    bool found = false;
    while(item < ITEMS_NUM) {
        const Item* current = Scenes[m->active_scene][item];

        if(abs(current->x - m->position) < 20) {
            found = !found;
            break;
        }
        ++item;
    }
    return found ? Scenes[m->active_scene][item] : NULL;
}

void draw_tv(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas,
        (TV.x + 5 - m->scene_offset) * PARALLAX(TV.layer),
        TV.y + 4,
        14,
        20);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_bitmap_mode(canvas, true);
}

void smash_tv(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    canvas_set_bitmap_mode(canvas, true);
    canvas_draw_icon_name(
        canvas,
        ((TV.x - 8) - m->scene_offset) * PARALLAX(TV.layer),
        TV.y - 6,
        I_FX_Bang_32x6);
    canvas_set_bitmap_mode(canvas, false);
    dolphin_draw_emote_bubble(canvas, model, "Bang!");
}

void sofa_sit(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    m->position = Sofa.x;
    m->animation = assets_icons_get(A_FX_Sitting_40x27);
    m->back = assets_icons_get(I_FX_SittingB_40x27);
    icon_start_animation(m->back);
}

void inspect_painting(Canvas* canvas, void* model) {
    char* emotes[5] = {
        "Debian?\nIt's a joke?",
        "Debian!\nIt's a joke!",
        "Debian...\nbut.. why?",
        "Alpine\nlinux\nbetter!"};

    dolphin_draw_emote_bubble(canvas, model, emotes[random() % 4]);
}

void pc_callback(Canvas* canvas, void* model) {
    char* emotes[5] = {
        "Loading...",
        "Welcome to\nSeaOS..",
        "Press any\nkey...",
    };

    dolphin_draw_emote_bubble(canvas, model, emotes[random() % 3]);
}