#include <furi.h>
#include "scene.h"
#include "assets/items.h"
#include "assets/meta.h"
#include <gui/elements.h>

const char* action_str[] = {"Idle", "Emote", "Use", "MC"};

void dolphin_scene_render_dolphin(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    FrameGroupsEnum group = GroupRight;
    FrameTypeEnum frame = FrameRight;

    // horizontal movement
    if(state->player_v.y == 0) {
        if(state->player_v.x < 0 || state->player_flipped_x) {
            group = state->transition ? GroupRight : GroupLeft;
            frame = FrameLeft;

        } else if(state->player_v.x > 0 || !state->player_flipped_x) {
            group = state->transition ? GroupLeft : GroupRight;
            frame = FrameRight;
        }
    } else {
        // vertical movement
        if(state->player_v.y < 0) {
            group = GroupUp;
            frame = FrameUp;

            if(state->transition) {
                frame = state->player_flipped_x ? FrameLeft : FrameRight;
            }
        } else if(state->player_v.y > 0) {
            group = GroupDown;
            frame = FrameDown;

            if(state->transition) {
                frame = state->player_flipped_x ? FrameLeft : FrameRight;
            }
        }
    }

    state->current_frame = *&frames[group][frame];

    if(state->current_frame->frames[state->frame_idx].f != NULL) {
        state->dolphin_gfx = state->current_frame->frames[state->frame_idx].f;
        state->dolphin_gfx_b = state->current_frame->frames[state->frame_idx].b;
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, state->player.x - 1, state->player.y - 1, state->dolphin_gfx_b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, state->player.x, state->player.y, state->dolphin_gfx);
    canvas_set_bitmap_mode(canvas, false);
}

static bool item_screen_bounds(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}

void dolphin_scene_render(SceneState* state, Canvas* canvas, uint32_t t) {
    furi_assert(state);
    furi_assert(canvas);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    const Item** current_scene = get_scene(state);

    for(uint8_t l = 0; l < LAYERS; l++) {
        if(state->scene_zoom < SCENE_ZOOM) {
            for(uint8_t i = 0; i < ItemsEnumTotal; i++) {
                int32_t item_pos_X = (current_scene[i]->pos.x - state->player_global.x);
                int32_t item_pos_Y = (current_scene[i]->pos.y - state->player_global.y);

                if(item_screen_bounds(item_pos_X)) {
                    if(l == current_scene[i]->layer) {
                        if(current_scene[i]->icon) {
                            canvas_draw_icon(
                                canvas,
                                item_pos_X * PARALLAX(l),
                                item_pos_Y,
                                current_scene[i]->icon);
                            canvas_set_bitmap_mode(canvas, false);
                        }

                        if(current_scene[i]->draw) {
                            current_scene[i]->draw(canvas, state);
                        }
                    }
                }
            }
        }

        if(l == DOLPHIN_LAYER) dolphin_scene_render_dolphin(state, canvas);
    }
}

void dolphin_scene_render_state(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    char buf[64];

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);

    // dolphin_scene_debug
    if(state->debug) {
        sprintf(
            buf,
            "%ld.%ld v:%ld.%ld; %d.%d %d %s %d",
            state->player_global.x,
            state->player_global.y,
            state->player_v.x,
            state->player_v.y,
            state->player_flipped_x,
            state->player_flipped_y,
            state->action_timeout,
            action_str[state->action],
            state->transition);
        canvas_draw_str(canvas, 0, 13, buf);
    }

    if(state->action == INTERACT) scene_activate_item_callback(state, canvas);
}