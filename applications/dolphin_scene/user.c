#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"

void render_dolphin(SceneState* state, Canvas* canvas) {
    IconName player_sprite;
    IconName player_background;

    if(state->player_v.x < 0) {
        if(state->player_anim == 0) {
            player_sprite = I_WalkL1_32x32;
            player_background = I_WalkLB1_32x32;

        } else {
            player_sprite = I_WalkL2_32x32;
            player_background = I_WalkLB2_32x32;
        }

    } else if(state->player_v.x > 0) {
        if(state->player_anim == 0) {
            player_sprite = I_WalkR1_32x32;
            player_background = I_WalkRB1_32x32;

        } else {
            player_sprite = I_WalkR2_32x32;
            player_background = I_WalkRB2_32x32;
        }

    } else {
        player_sprite = A_MDI_32x32;
        player_background = A_MDIB_32x32;
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);

    canvas_draw_icon_name(
        canvas, state->player.x / SCALE, state->player.y / SCALE - 5, player_background);

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon_name(
        canvas, state->player.x / SCALE, state->player.y / SCALE - 5, player_sprite);

    canvas_set_bitmap_mode(canvas, false);
}

void handle_user_input(SceneState* state, InputEvent* input) {
    if(input->type == InputTypePress) {
        if(input->key == InputKeyRight) {
            state->player_v.x = SPEED_X;
        } else if(input->key == InputKeyLeft) {
            state->player_v.x = -SPEED_X;
        }
    } else if(input->type == InputTypeRelease) {
        if(input->key == InputKeyRight || input->key == InputKeyLeft) {
            state->player_v.x = 0;
        }
    }
}

void update_dolphin_coordinates(SceneState* state, uint32_t dt) {
    state->player.y += state->player_v.y * dt;
    state->player_global.y += state->player_v.y * dt;

    if(state->player.x < BONDARIES_X_LEFT * SCALE) {
        state->player.x = BONDARIES_X_LEFT * SCALE;
    } else if(state->player.x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state->player.x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    } else {
        state->player.x += state->player_v.x * dt;
    }

    if(state->player.x <= BONDARIES_X_LEFT * SCALE) {
        if(!state->in_boundaries) {
            state->player_global.x += state->player_v.x * dt;
        }
    } else if(state->player.x >= (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        if(!state->in_boundaries) {
            state->player_global.x += state->player_v.x * dt;
        }
    } else {
        state->player_global.x += state->player_v.x * dt;
    }

    // global
    state->screen.x = state->player_global.x - state->player.x;

    if(state->player_global.x < BONDARIES_X_LEFT * SCALE) {
        state->player_global.x += WORLD_WIDTH * SCALE;
    }

    if(state->player_global.x > (WORLD_WIDTH - BONDARIES_X_RIGHT * SCALE)) {
        state->player_global.x -= WORLD_WIDTH * SCALE;
    }

    state->player_anim = (state->player_global.x / (SCALE * 8)) % 2;
}