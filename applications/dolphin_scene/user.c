#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"

void render_dolphin(SceneState* state, Canvas* canvas) {
    IconName player_sprite;
    IconName player_background;

    if(state->player_v.x < 0 || state->player_flipped) {
        if(state->player_anim == 0) {
            player_sprite = I_WalkL1_32x32;
            player_background = I_WalkLB1_32x32;

        } else {
            player_sprite = I_WalkL2_32x32;
            player_background = I_WalkLB2_32x32;
        }
    } else if(state->player_v.x > 0 || !state->player_flipped) {
        if(state->player_anim == 0) {
            player_sprite = I_WalkR1_32x32;
            player_background = I_WalkRB1_32x32;

        } else {
            player_sprite = I_WalkR2_32x32;
            player_background = I_WalkRB2_32x32;
        }
    }

    // zoom handlers
    if(state->scene_zoom == SCENE_ZOOM) {
        player_sprite = I_DolphinExcited_64x63;
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);

    canvas_draw_icon_name(canvas, state->player.x, state->player.y, player_background);

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon_name(canvas, state->player.x, state->player.y, player_sprite);

    canvas_set_bitmap_mode(canvas, false);
}

void handle_user_input(SceneState* state, InputEvent* input) {
    if(input->type == InputTypePress) {
        if(input->key == InputKeyRight) {
            state->player_flipped = false;
            state->player_v.x = SPEED_X;
        } else if(input->key == InputKeyLeft) {
            state->player_flipped = true;
            state->player_v.x = -SPEED_X;
        } else if(input->key == InputKeyOk) {
            state->zoom_v = SPEED_X;
        }
    } else if(input->type == InputTypeRelease) {
        if(input->key == InputKeyRight || input->key == InputKeyLeft) {
            state->player_v.x = 0;
        } else if(input->key == InputKeyOk) {
            state->zoom_v = -SPEED_X * 2;
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