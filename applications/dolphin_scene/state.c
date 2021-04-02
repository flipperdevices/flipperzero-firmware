#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#define ACTION_N 4 // no random mind control
enum Actions { SLEEP = 0, IDLE, WALK, EMOTE, USE, MINDCONTROL };

static uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

void update_dolphin_state(SceneState* state, uint32_t t, uint32_t dt) {
    //int32_t global_x = (state->player_global.x / SCALE) % WORLD_WIDTH;

    if(state->action_timeout > 0) {
        state->action_timeout--;
    } else {
        if(state->action != MINDCONTROL && random() % 1000 > 500) {
            state->action_timeout = 100;
            state->next_action = roll_new(state->previous_action, ACTION_N);

            if(state->action != SLEEP) {
                if(state->player_global.x == state->poi) {
                    //state->action_timeout = 100;
                    state->poi = random() % 255;
                }
            }
        }
    }

    //if( state->use_item) {
    //    dolphin_use_item(canvas, model);
    //     state->use_item = false;
    //}
    /*
    switch(state->action) {
    case WALK:
        if(global_x < (state->poi / SCALE) % WORLD_WIDTH) {
            state->player_v.x = SPEED_X;
        }
        if(global_x > (state->poi / SCALE) % WORLD_WIDTH) {
            state->player_v.x = -SPEED_X;
        }

        break;

    case EMOTE:
        if( state->action_timeout == 0 &&  state->previous_action != EMOTE) {
             state->action_timeout = 50;
             state->emote_id = roll_new( state->previous_emote, ARRSIZE(emotes_list));
        }

        else if( state->previous_action !=  state->action) {
            dolphin_draw_emote_bubble(canvas, model, NULL);
        } else {
             state->action = IDLE;
        }

        break;
    case MINDCONTROL:
        break;

    default:
        // state->action_timeout = 0;
        state->previous_action = state->action;
        state->action = state->next_action;
        break;
    }
    */
}

void render_dolphin_state(SceneState* state, Canvas* canvas) {
    /*
    char buf[32];

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);

    sprintf(buf, "x: %ld", (state->player_global.x / SCALE) % WORLD_WIDTH);
    canvas_draw_str(canvas, 0, 40, buf);
    */
}