#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/items.h"

const Item* Home[] = {&TV, &Sofa, &Painting, &PC};
const Item** Scenes[] = {*&Home};

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

static bool item_screen_bounds(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}

static void draw_hint(SceneState* state, Canvas* canvas) {
    const Item* near = is_nearby(state);
    if(near) {
        canvas_draw_str(
            canvas, 127 - canvas_string_width(canvas, near->action_name), 8, near->action_name);
    }
}

void render_scene(SceneState* state, Canvas* canvas, uint32_t t) {
    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t l = 0; l < LAYERS; l++) {
        uint8_t active = 0;
        if(state->scene_zoom < SCENE_ZOOM) {
            for(uint8_t i = 0; i < ITEMS_NUM; i++) {
                const Item* current = Scenes[active][i];
                int32_t item_pos = (current->x - state->player_global.x);

                if(item_screen_bounds(item_pos)) {
                    if(current->draw) {
                        current->draw(canvas, state);
                    }

                    if(l == current->layer) {
                        canvas_draw_icon_name(
                            canvas, item_pos * PARALLAX(l), current->y + 8, current->icon);
                        canvas_set_bitmap_mode(canvas, false);
                    }
                }
            }

            if(l == 0) canvas_draw_line(canvas, 0, 50, 128, 50);
        }

        if(l == DOLPHIN_LAYER) render_dolphin(state, canvas);
    }

    draw_hint(state, canvas);
}
