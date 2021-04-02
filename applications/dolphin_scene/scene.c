#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/items.h"

static const Item* Home[] = {&TV, &Sofa, &Painting, &PC};
static const Item** Scenes[] = {*&Home};

void render_scene(SceneState* state, Canvas* canvas, uint32_t t) {

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

}
