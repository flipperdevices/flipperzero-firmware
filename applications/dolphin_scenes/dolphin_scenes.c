#include <furi.h>
#include "dolphin_scenes/dolphin_scenes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void scene_show(SceneState* state) {
    furi_assert(state);

    if(!state->enabled) {
        view_port_enabled_set(state->view_port, true);
        return;
    }
    return;
    //gui_send_view_port_back(menu->gui, menu->view_port);
}

void scene_redraw(Canvas* canvas, void* ctx) {
    SceneState* state = (SceneState*)acquire_mutex((ValueMutex*)ctx, 25);
    uint32_t t = xTaskGetTickCount();

    canvas_clear(canvas);

    render_scene(state, canvas, t);
    //render_player(state, canvas);
    render_dolphin_state(state, canvas);

    release_mutex((ValueMutex*)ctx, state);
}

void handle_input(SceneState* state, InputEvent* input) {
    // printf("[kb] event: %02x %s\n", input->key, input->state ? "pressed" : "released");
    handle_user_input(state, input);
}

void tick_handler(SceneState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    update_dolphin_coordinates(state, dt);
    update_dolphin_state(state, t, dt);
}
