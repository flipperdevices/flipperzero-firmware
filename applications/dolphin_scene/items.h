#pragma once

#include <gui/gui.h>

static const Item TV = {
    .layer = 7,
    .x = 40,
    .y = 34,
    .icon = I_TV_20x24,
    .action_name = "Use",
    .draw = NULL,
    .callback = NULL};

static const Item Painting = {
    .layer = 3,
    .x = 65,
    .y = 10,
    .icon = I_Home_painting_17x20,
    .action_name = "Inspect",
    .draw = NULL,
    .callback = NULL};

static const Item Sofa = {
    .layer = 3,
    .x = 150,
    .y = 34,
    .icon = I_Sofa_40x13,
    .action_name = "Sit",
    .draw = NULL,
    .callback = NULL};

static const Item PC = {
    .layer = 3,
    .x = 300,
    .y = 10,
    .icon = I_PC_22x29,
    .action_name = "Use",
    .draw = NULL,
    .callback = NULL};

/*
const Item* is_nearby(void* model) {};
    //furi_assert(model);
    //DolphinViewMetaModel* m = model;
    //uint8_t item = 0;
    //bool found = false;
    //while(item < ITEMS_NUM) {
    //    const Item* current = Scenes[m->active_scene][item];

    //    if(abs(current->x - m->position) < 20) {
    //        found = !found;
    //        break;
    //    }
     //   ++item;
    //}
    //return found ? Scenes[m->active_scene][item] : NULL;
//}

void draw_tv(Canvas* canvas, void* model) {
    furi_assert(model);
  //  DolphinViewMetaModel* m = model;
    canvas_set_color(canvas, ColorWhite);
   // canvas_draw_box(canvas, (TV.x + 5 - m->scene_offset) * PARALLAX(TV.layer), TV.y + 4, 14, 20);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_bitmap_mode(canvas, true);
}

void smash_tv(Canvas* canvas, void* model) {
    furi_assert(model);
   // DolphinViewMetaModel* m = model;
    canvas_set_bitmap_mode(canvas, true);
   // canvas_draw_icon_name(
   //     canvas, ((TV.x - 8) - m->scene_offset) * PARALLAX(TV.layer), TV.y - 6, I_FX_Bang_32x6);
    canvas_set_bitmap_mode(canvas, false);
   // dolphin_draw_emote_bubble(canvas, model, "Bang!");
}

void sofa_sit(Canvas* canvas, void* model) {
    furi_assert(model);
   /// DolphinViewMetaModel* m = model;

    //m->animation = assets_icons_get(A_FX_Sitting_40x27);
    //m->back = assets_icons_get(I_FX_SittingB_40x27);
    //icon_start_animation(m->back);
}

void inspect_painting(Canvas* canvas, void* model) {
    furi_assert(model);
   // dolphin_draw_emote_bubble(canvas, model, NULL);
}

void pc_callback(Canvas* canvas, void* model) {
    furi_assert(model);
    //dolphin_draw_emote_bubble(canvas, model, NULL);
}
*/