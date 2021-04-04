#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"
#include "dolphin_scene/dolphin_emotions.h"
#include <gui/elements.h>

const char* action_str[] = {"Sleep", "Idle", "Walk", "Emote", "Use", "Mindcontrol"};

static uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

void update_dolphin_state(SceneState* state, uint32_t t, uint32_t dt) {
    int32_t global_x = state->player_global.x;

    if(state->action_timeout > 0) {
        state->action_timeout--;
    } else {
        if(state->action != MINDCONTROL && random() % 1000 > 500) {
            state->action_timeout = 100;
            state->next_action =
                roll_new(state->prev_action, ACTIONS_NUM - 1); // no random mind control
        }
    }

    switch(state->action) {
    case WALK:
        if(global_x < state->poi) {
            state->player_flipped = false;
            state->player_v.x = SPEED_X / 2;
        } else if(global_x > state->poi) {
            state->player_flipped = true;
            state->player_v.x = -SPEED_X / 2;
        } else {
            state->player_v.x = 0;
            state->poi = roll_new(state->player_global.x, WORLD_WIDTH);
            state->prev_action = WALK;
            state->action = IDLE;
        }
        break;

    case MINDCONTROL:
        break;

    default:

        state->prev_action = state->action != state->next_action ? state->action :
                                                                   state->next_action;
        state->action = state->next_action;
        break;
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

    if(state->scene_zoom == SCENE_ZOOM) {
        elements_multiline_text_framed(canvas, 68, 25, "Let's hack!\n\nbla bla bla\nbla bla..");
    }
}