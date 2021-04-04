

#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/items.h"
#include <gui/gui.h>

const Item* is_nearby(SceneState* state) {
    furi_assert(state);
    uint8_t item = 0;
    bool found = false;
    while(item < ITEMS_NUM) {
        const Item* current = Scenes[state->scene_id][item];
        int32_t rel =
            (SCREEN_WIDTH / 2 - (current->x - state->player_global.x) * PARALLAX(current->layer));
        if(abs(rel) <= DOLPHIN_WIDTH / 2) {
            found = !found;
            break;
        }
        ++item;
    }
    return found ? Scenes[state->scene_id][item] : NULL;
}

void draw_tv(Canvas* canvas, void* model) {
    furi_assert(model);
    SceneState* m = model;
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas, (TV.x + 5 - m->player_global.x) * PARALLAX(TV.layer), TV.y + 4, 14, 20);
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
