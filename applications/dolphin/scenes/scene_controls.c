#include <furi.h>
#include <gui/elements.h>
#include "scene.h"

void dolphin_scene_handle_user_input(SceneState* state, InputEvent* input) {
    furi_assert(state);
    furi_assert(input);
    // toggle mind control on any user interaction
    if(input->type == InputTypePress) {
        state->action = MINDCONTROL;
    }

    // mind control
    if(state->action == MINDCONTROL) {
        if(input->type == InputTypePress) {
            if(input->key == InputKeyRight) {
                state->player_v.y = 0;
                state->player_v.x = SPEED_X;
            } else if(input->key == InputKeyLeft) {
                state->player_v.y = 0;
                state->player_v.x = -SPEED_X;
            } else if(input->key == InputKeyUp) {
                state->player_v.x = 0;
                state->player_v.y = -SPEED_Y;
            } else if(input->key == InputKeyDown) {
                state->player_v.x = 0;
                state->player_v.y = SPEED_Y;
            }
            state->last_group = state->frame_group;
        }

        if(input->type == InputTypeRelease) {
            state->player_v.x = 0;
            state->player_v.y = 0;
        } else if(input->type == InputTypeShort) {
            if(input->key == InputKeyOk) {
                state->prev_action = MINDCONTROL;
                state->action = INTERACT;
                state->use_pending = true;
                state->action_timeout = 0;
            }
        }
    }
}

void dolphin_scene_coordinates(SceneState* state, uint32_t dt) {
    furi_assert(state);

    uint8_t speed_mod = (state->player_v.x != 0 || state->player_v.y != 0) ? SPEED_X : SPEED_X * 2;
    // global pos
    state->player_global.x = CLAMP(state->player_global.x + state->player_v.x, WORLD_WIDTH, 0);
    state->player_global.y = CLAMP(state->player_global.y + state->player_v.y, WORLD_HEIGHT, 0);

    // nudge camera postition
    if(state->player_global.x > 170) {
        state->player.x =
            CLAMP(state->player.x - state->player_v.x / 2, DOLPHIN_CENTER, -DOLPHIN_WIDTH / 2);
    } else if(state->player_global.x < 70) {
        state->player.x =
            CLAMP(state->player.x - state->player_v.x / 2, DOLPHIN_WIDTH * 2, DOLPHIN_CENTER);
    }

    ++state->player_anim;

    if(state->player_v.y < 0 && !state->player_flipped_y) {
        state->transition = true;
        state->player_flipped_y = true;
        state->player_anim = 0;
        state->frame_idx = 0;
    } else if(state->player_v.y > 0 && state->player_flipped_y) {
        state->transition = true;
        state->player_flipped_y = false;
        state->player_anim = 0;
        state->frame_idx = 0;
    } else if(state->player_v.x < 0 && !state->player_flipped_x) {
        state->transition = true;
        state->player_flipped_x = true;
        state->player_anim = 0;
        state->frame_idx = 0;
    } else if(state->player_v.x > 0 && state->player_flipped_x) {
        state->transition = true;
        state->player_flipped_x = false;
        state->player_anim = 0;
        state->frame_idx = 0;
    }

    if(state->player_anim == state->current_frame->total && state->transition) {
        state->transition = false;
    }

    if(state->player_anim % speed_mod == 0) {
        state->frame_idx = (state->frame_idx + 1) % state->current_frame->total;
    }
}