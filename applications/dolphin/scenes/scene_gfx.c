#include <furi.h>
#include "scene.h"
#include "assets/items.h"
#include "assets/meta.h"
#include <gui/elements.h>

const char* action_str[] = {"Idle", "Emote", "Use", "MC"};

void dolphin_scene_render_dolphin(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    if(state->player_v.x < 0) {
        state->frame_group = GroupLeft;

        if(state->transition) {
            if(state->last_group == GroupDown) {
                state->frame_type = FrameDown;
            } else if(state->last_group == GroupUp) {
                state->frame_type = FrameUp;
            } else if(state->last_group == GroupRight) {
                state->frame_type = FrameRight;
            }
        } else {
            state->frame_type = FrameLeft;
        }
    } else if(state->player_v.x > 0) {
        state->frame_group = GroupRight;

        if(state->transition) {
            if(state->last_group == GroupDown) {
                state->frame_type = FrameDown;
            } else if(state->last_group == GroupLeft) {
                state->frame_type = FrameLeft;
            } else if(state->last_group == GroupUp) {
                state->frame_type = FrameUp;
            }
        } else {
            state->frame_type = FrameRight;
        }

    } else if(state->player_v.y < 0) {
        state->frame_group = GroupUp;

        if(state->transition) {
            if(state->last_group == GroupDown) {
                state->frame_type = FrameDown;
            } else if(state->last_group == GroupLeft) {
                state->frame_type = FrameLeft;
            } else if(state->last_group == GroupRight) {
                state->frame_type = FrameRight;
            }
        } else {
            state->frame_type = FrameUp;
        }
    } else if(state->player_v.y > 0) {
        state->frame_group = GroupDown;

        if(state->transition) {
            if(state->last_group == GroupUp) {
                state->frame_type = FrameUp;
            } else if(state->last_group == GroupLeft) {
                state->frame_type = FrameLeft;
            } else if(state->last_group == GroupRight) {
                state->frame_type = FrameRight;
            }
        } else {
            state->frame_type = FrameDown;
        }
    }

    if(*&frames[state->frame_group][state->frame_type]->frames[state->frame_idx].f != NULL) {
        state->current_frame = *&frames[state->frame_group][state->frame_type];
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(
        canvas, state->player.x, state->player.y, state->current_frame->frames[state->frame_idx].b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(
        canvas, state->player.x, state->player.y, state->current_frame->frames[state->frame_idx].f);
    canvas_set_bitmap_mode(canvas, false);
}

static bool item_screen_bounds_x(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}
static bool item_screen_bounds_y(int32_t pos) {
    return pos > -SCREEN_HEIGHT * 2 && pos < (SCREEN_HEIGHT * 2);
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

                if(l == 0) {
                    if(item_screen_bounds_x(0 - state->player_global.x) &&
                       item_screen_bounds_y(0 - state->player_global.y)) {
                        canvas_draw_icon(
                            canvas,
                            0 - state->player_global.x,
                            0 - state->player_global.y,
                            &I_map1_128x128);
                    }

                    if(item_screen_bounds_x(128 - state->player_global.x) &&
                       item_screen_bounds_y(0 - state->player_global.y)) {
                        canvas_draw_icon(
                            canvas,
                            128 - state->player_global.x,
                            0 - state->player_global.y,
                            &I_map2_128x128);
                    }

                    if(item_screen_bounds_x(0 - state->player_global.x) &&
                       item_screen_bounds_y(124 - state->player_global.y)) {
                        canvas_draw_icon(
                            canvas,
                            0 - state->player_global.x,
                            124 - state->player_global.y,
                            &I_map3_128x128);
                    }

                    if(item_screen_bounds_x(128 - state->player_global.x) &&
                       item_screen_bounds_y(124 - state->player_global.y)) {
                        canvas_draw_icon(
                            canvas,
                            128 - state->player_global.x,
                            124 - state->player_global.y,
                            &I_map4_128x128);
                    }

                    if(state->player_global.x > 128 &&
                       item_screen_bounds_y(124 - state->player_global.y)) {
                        canvas_draw_icon(
                            canvas,
                            240 - state->player_global.x,
                            124 - state->player_global.y,
                            &I_map5_128x128);
                    }
                }

                if(item_screen_bounds_x(item_pos_X) && item_screen_bounds_y(item_pos_Y)) {
                    if(l == current_scene[i]->layer) {
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
            "%ld.%ld v:%d.%d; %d %d %s %d",
            state->player_v.x,
            state->player_v.y,
            state->frame_group,
            state->frame_type,
            state->last_group,
            state->action_timeout,
            action_str[state->action],
            state->transition);
        canvas_draw_str(canvas, 0, 13, buf);
    }

    if(state->action == INTERACT) scene_activate_item_callback(state, canvas);
}