#include <furi.h>
#include "dolphin_scenes/dolphin_scenes.h"
#include "dolphin_scenes/items.h"

static const Item* Home[] = {&TV, &Sofa, &Painting, &PC};
static const Item** Scenes[] = {*&Home};

void render_scene(SceneState* state, Canvas* canvas, uint32_t t) {
    canvas_set_color(canvas, ColorBlack);
    //int32_t floor_height = 18000;
    //canvas_draw_box(canvas, 0, SCREEN_HEIGHT - (floor_height - state->screen.y) / SCALE, SCREEN_WIDTH, 1);

    // in-level label
    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);

    if(t - state->glitch_t > 250) {
        state->glitch_t = t;
    }

    for(uint8_t l = 0; l < LAYERS; l++) {
        uint8_t active = 0;
        for(uint8_t i = 0; i < ITEMS_NUM; i++) {
            if(Scenes[active][i]->draw) {
                Scenes[active][i]->draw(canvas, state);
            }

            if(l == Scenes[active][i]->layer) {
                canvas_draw_icon_name(
                    canvas,
                    ((Scenes[active][i]->x - state->player_global.x / SCALE) * PARALLAX(l)),
                    Scenes[active][i]->y + 8,
                    Scenes[active][i]->icon);
                canvas_set_bitmap_mode(canvas, false);
            }
        }
        if(l == DOLPHIN_LAYER) render_dolphin(state, canvas); //draw_dolphin(canvas, state);

        if(l == 0) canvas_draw_line(canvas, 0, 50, 128, 50);
    }

    /*
    const TextBlock* label = &NARRATIVE[state->label_id];
    for(size_t i = 0; i < label->line_size; i++) {
        strcpy(buf, label->lines[i]);

        for(size_t glitch = 0; glitch < state->glitch_level; glitch++) {
            buf[(state->glitch_t + glitch * 23) % strlen(buf)] =
                ' ' + (state->glitch_t + glitch * 17) % ('z' - ' ');
        }

        canvas_draw_str(
            canvas,
            (LABEL_X - state->screen.x) / SCALE,
            ((LABEL_Y + LABEL_HEIGHT * i) + state->screen.y) / SCALE,
            buf);
    }
    */
}
