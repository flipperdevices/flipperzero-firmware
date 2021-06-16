#include <furi.h>
#include "scene.h"
#include "assets/emotes.h"

static uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

static void scene_proceed_action(SceneState* state) {
    furi_assert(state);

    state->prev_action = state->action;

    state->action = roll_new(state->prev_action, ACTIONS_NUM);

    state->action_timeout = default_timeout[state->action];
}

static bool scene_dolphin_on_x_axis(SceneState* state) {
    return abs(state->player_global.x - state->poi.x) <= 2;
}

static bool scene_dolphin_on_y_axis(SceneState* state) {
    return abs(state->player_global.y - state->poi.y) <= 2;
}

static bool scene_dolphin_on_poi(SceneState* state) {
    return (abs(state->player_global.x - state->poi.x) <= 2) &&
           (abs(state->player_global.y - state->poi.y) <= 2);
}

static void scene_dolphin_go_to_poi(SceneState* state) {
    furi_assert(state);
    if(!scene_dolphin_on_x_axis(state)) {
        if(state->player_global.x < state->poi.x) {
            state->player_flipped = false;
            state->player_v.x = SPEED_X / 2;
        } else if(state->player_global.x > state->poi.x) {
            state->player_flipped = true;
            state->player_v.x = -SPEED_X / 2;
        }
    }

    if(!scene_dolphin_on_y_axis(state)) {
        if(state->player_global.y < state->poi.y) {
            state->player_v.y = SPEED_Y / 2;
        } else if(state->player_global.y > state->poi.y) {
            state->player_v.y = -SPEED_Y / 2;
        }
    }
}

static void scene_action_handler(SceneState* state) {
    furi_assert(state);
    if(state->action == MINDCONTROL) {
        if(state->player_v.x != 0 || state->player_v.y != 0) {
            state->action_timeout = default_timeout[state->action];
        }
    }

    if(state->action_timeout > 0) {
        state->action_timeout--;
    } else {
        if(random() % 1000 > 500) {
            state->next_action = roll_new(state->prev_action, ACTIONS_NUM);
            state->poi.x = roll_new(state->player_global.x, WORLD_WIDTH);
            state->poi.y = roll_new(state->player_global.y, WORLD_HEIGHT);
        }
    }
}

void dolphin_scene_update_state(SceneState* state, uint32_t t, uint32_t dt) {
    furi_assert(state);
    scene_action_handler(state);
    UNUSED(dialogues_list);

    switch(state->action) {
    case WALK:
        if(state->player_global.x == state->poi.x && state->player_global.y == state->poi.y) {
            state->player_v.x = 0;
            state->player_v.y = 0;
        } else if(state->action_timeout == 0) {
            scene_proceed_action(state);
        } else {
            scene_dolphin_go_to_poi(state);
        }
        break;
    case EMOTE:
        state->player_flipped = false;
        if(state->action_timeout == 0) {
            scene_proceed_action(state);
            state->emote_id = roll_new(state->previous_emote, SIZEOF_ARRAY(emotes_list));
            break;
        }
    case INTERACT:
        if(state->action_timeout == 0) {
            if(state->prev_action == MINDCONTROL) {
                state->action = MINDCONTROL;
            } else {
                scene_proceed_action(state);
            }
        }
        break;
    case SLEEP:
        if(state->poi.x != 154 || state->poi.y != 52) { // temp
            state->poi.x = 154;
            state->poi.y = 52;
        } else if(!scene_dolphin_on_poi(state)) {
            scene_dolphin_go_to_poi(state);
        } else {
            state->player_v.x = 0;
            state->player_v.y = 0;

            if(state->action_timeout == 0) {
                state->poi.x = roll_new(state->player_global.x, WORLD_WIDTH);
                state->poi.y = roll_new(state->player_global.y, WORLD_HEIGHT);
                scene_proceed_action(state);
            }

            break;
        }
    default:
        if(state->action_timeout == 0) {
            scene_proceed_action(state);
        }
        break;
    }
}
