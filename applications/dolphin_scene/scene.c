#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/items.h"
#include <gui/elements.h>

const char* action_str[] = {"Sleep", "Idle", "Walk", "Emote", "Use", "MC"};

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

void activate_item_callback(SceneState* state, Canvas* canvas){
    const Item* near = is_nearby(state);
    if(near){
        state->action = INTERACT;
        near->callback(canvas, state);
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

    if(state->action == INTERACT) activate_item_callback(state, canvas);

    if(state->action == MINDCONTROL) draw_hint(state, canvas);
}

void render_dolphin_state(SceneState* state, Canvas* canvas) {
    char buf[64];

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    sprintf(
        buf,
        "x:%ld>%d %ld %s",
        state->player_global.x,
        state->poi,
        state->action_timeout,
        action_str[state->action]);
    //sprintf(buf, "x:%ld s:%ld p:%ld %d %s", state->player_global.x, state->screen.x, state->player.x, state->scene_zoom, action_str[state->action]);
    canvas_draw_str(canvas, 0, 8, buf);

    if(state->scene_zoom == SCENE_ZOOM) {
        elements_multiline_text_framed(canvas, 68, 25, "Let's hack!\n\nbla bla bla\nbla bla..");
    }
}