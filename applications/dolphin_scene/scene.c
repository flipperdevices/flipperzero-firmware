#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/dolphin_emotes.h"
#include "dolphin_scene/items.h"
#include <gui/elements.h>

const char* action_str[] = {"Sleep", "Idle", "Walk", "Emote", "Use", "MC"};

static bool item_screen_bounds(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}

static void draw_hint(SceneState* state, Canvas* canvas) {
    const Item* near = is_nearby(state);
    if(near) {
        int32_t hint_pos_x = (near->x - state->player_global.x) * PARALLAX(near->layer) + 20;
        int8_t hint_pos_y = near->y < 15 ? near->y + 8 : near->y - 12;
        canvas_draw_str(canvas, hint_pos_x, hint_pos_y, near->action_name);
    }
}

static void draw_current_emote(SceneState* state, Canvas* canvas) {
    elements_multiline_text_framed(canvas, 80, 24, (char*)emotes_list[state->emote_id]);
}

static void draw_sleep_emote(SceneState* state, Canvas* canvas) {
    if(state->player_global.x == 154 && state->action_timeout % 100 < 40) {
        elements_multiline_text_framed(canvas, 80, 24, "zZzZ..");
    }
}

static void draw_dialog(SceneState* state, Canvas* canvas) {
    elements_multiline_text_framed(canvas, 68, 25, "Let's hack!\n\nbla bla bla\nbla bla..");
}

void activate_item_callback(SceneState* state, Canvas* canvas) {
    const Item* near = is_nearby(state);
    if(near && state->use_pending == true) {
        state->action_timeout = near->timeout;
        near->callback(canvas, state);
        state->use_pending = false;
    } else if(near) {
        near->callback(canvas, state);
    }
}

void render_scene(SceneState* state, Canvas* canvas, uint32_t t) {
    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    const Item** current_scene = get_scene(state);

    for(uint8_t l = 0; l < LAYERS; l++) {
        if(state->scene_zoom < SCENE_ZOOM) {
            for(uint8_t i = 0; i < ITEMS_NUM; i++) {
                int32_t item_pos = (current_scene[i]->x - state->player_global.x);
                if(item_screen_bounds(item_pos)) {
                    if(current_scene[i]->draw) current_scene[i]->draw(canvas, state);

                    if(l == current_scene[i]->layer) {
                        canvas_draw_icon_name(
                            canvas,
                            item_pos * PARALLAX(l),
                            current_scene[i]->y + 8,
                            current_scene[i]->icon);
                        canvas_set_bitmap_mode(canvas, false);
                    }
                }
            }

            if(l == 0) canvas_draw_line(canvas, 0, 50, 128, 50);
        }

        if(l == DOLPHIN_LAYER) render_dolphin(state, canvas);
    }
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

    if(state->scene_zoom == SCENE_ZOOM)
        draw_dialog(state, canvas);
    else if(state->action == EMOTE)
        draw_current_emote(state, canvas);
    else if(state->action == MINDCONTROL)
        draw_hint(state, canvas);
    else if(state->action == INTERACT)
        activate_item_callback(state, canvas);
    else if(state->action == SLEEP)
        draw_sleep_emote(state, canvas);
}