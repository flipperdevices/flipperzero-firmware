#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/dolphin_emotes.h"

static uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}


static void dolphin_actions_proceed(SceneState* state){
    state->prev_action = state->action != state->next_action ? state->action :
                                                            state->next_action;
    state->action = state->next_action;
}


static void dolphin_go_to_poi(SceneState* state){
    if(state->player_global.x < state->poi) {
        state->player_flipped = false;
        state->player_v.x = SPEED_X / 2;
    } else if(state->player_global.x > state->poi) {
        state->player_flipped = true;
        state->player_v.x = -SPEED_X / 2;
    }
}



void update_dolphin_state(SceneState* state, uint32_t t, uint32_t dt) {

    if(state->action == MINDCONTROL && state->player_v.x != 0) {
        state->action_timeout = default_timeout[state->action];
    }



    if(state->action_timeout > 0) {
        state->action_timeout--;
    }else {
        state->action_timeout = default_timeout[state->action];

        if(random() % 1000 > 500) {
            state->next_action =
                roll_new(state->prev_action, ACTIONS_NUM);
        }
    }

    switch(state->action) {

    case WALK:
        if(state->player_global.x == state->poi){
            state->poi = roll_new(state->player_global.x, WORLD_WIDTH / 4);
            state->player_v.x = 0;
            dolphin_actions_proceed(state);
        } else {
            dolphin_go_to_poi(state);
        }
        break;
    case EMOTE:
        state->player_flipped = false;
        if(state->action_timeout == 0){
            state->emote_id = roll_new(state->previous_emote, ARRSIZE(emotes_list));
        }else{
            dolphin_actions_proceed(state);
        }
        break;
    case INTERACT:
        if(state->action_timeout == 0) {
            if(state->prev_action == MINDCONTROL){
                state->action = MINDCONTROL;
            }else{
                dolphin_actions_proceed(state);
            }
        }
        break;
    //case SLEEP:
    //    if(state->player_global.x != )
    default:
        if(state->action_timeout == 0) {
            dolphin_actions_proceed(state);
        }
        break;
    }


}

