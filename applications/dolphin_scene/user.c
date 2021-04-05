#include <furi.h>
#include <gui/elements.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/dolphin_emotes.h"

void render_dolphin(SceneState* state, Canvas* canvas) {
    if(state->action != INTERACT) {
        if(state->player_v.x < 0 || state->player_flipped) {
            if(state->player_anim == 0) {
                state->dolphin_gfx = I_WalkL1_32x32;
                state->dolphin_gfx_b = I_WalkLB1_32x32;

            } else {
                state->dolphin_gfx = I_WalkL2_32x32;
                state->dolphin_gfx_b = I_WalkLB2_32x32;
            }
        } else if(state->player_v.x > 0 || !state->player_flipped) {
            if(state->player_anim == 0) {
                state->dolphin_gfx = I_WalkR1_32x32;
                state->dolphin_gfx_b = I_WalkRB1_32x32;

            } else {
                state->dolphin_gfx = I_WalkR2_32x32;
                state->dolphin_gfx_b = I_WalkRB2_32x32;
            }
        }
    }

    if(state->action == SLEEP && state->player_v.x == 0) {
        state->dolphin_gfx = A_FX_Sitting_40x27;
        state->dolphin_gfx_b = I_FX_SittingB_40x27;
    }
    // zoom handlers
    if(state->scene_zoom == SCENE_ZOOM) {
        state->dolphin_gfx = I_DolphinExcited_64x63;
    }

    if(state->action == EMOTE) {
        elements_multiline_text_framed(canvas, 80, 24, (char*)emotes_list[state->emote_id]);
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon_name(canvas, state->player.x, state->player.y, state->dolphin_gfx_b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon_name(canvas, state->player.x, state->player.y, state->dolphin_gfx);
    canvas_set_bitmap_mode(canvas, false);
}

void handle_user_input(SceneState* state, InputEvent* input) {
    if(input->type == InputTypePress) {
        if(input->key == InputKeyLeft || input->key == InputKeyRight) {
            state->action = MINDCONTROL;
        }
    }

    // zoom tests
    if(input->type == InputTypePress) {
        if(input->key == InputKeyDown) {
            state->zoom_v = SPEED_X;
        }
    } else if(input->type == InputTypeRelease) {
        if(input->key == InputKeyDown) {
            state->zoom_v = -SPEED_X * 2;
        }
    }

    if(state->action == MINDCONTROL) {
        if(input->type == InputTypePress) {
            if(input->key == InputKeyRight) {
                state->player_flipped = false;
                state->player_v.x = SPEED_X;
            } else if(input->key == InputKeyLeft) {
                state->player_flipped = true;
                state->player_v.x = -SPEED_X;
            }
        } else if(input->type == InputTypeRelease) {
            if(input->key == InputKeyRight || input->key == InputKeyLeft) {
                state->player_v.x = 0;
            }
        } else if(input->type == InputTypeShort) {
            if(input->key == InputKeyOk) {
                state->prev_action = MINDCONTROL;
                state->action = INTERACT;
                state->action_timeout = 0;
            }
        }
    }
}

void update_dolphin_coordinates(SceneState* state, uint32_t dt) {
    // global pos
    state->player_global.x = CLAMP(state->player_global.x + state->player_v.x, WORLD_WIDTH, 0);

    // zoom handlers
    state->scene_zoom = CLAMP(state->scene_zoom + state->zoom_v, SCENE_ZOOM, 0);
    state->player.x = CLAMP(state->player.x - (state->zoom_v * (SPEED_X * 2)), DOLPHIN_CENTER, 0);
    state->player.y =
        CLAMP(state->player.y - (state->zoom_v * SPEED_X / 2), DOLPHIN_DEFAULT_Y, 10);

    //center screen
    state->screen.x = state->player_global.x - state->player.x;
    state->player_anim = (state->player_global.x / 10) % 2;
}