#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include "utils.h"
#include "scenes/scenes.h"
#include "../constants.h"

void start_gui_and_block(struct ApplicationContext* context) {
    // Select the first scene to launch
    scene_manager_next_scene(context->scene_manager, scene_loading);

    /* Blocking call until the back button is pressed from the main view */
    FURI_LOG_D(LOG_TAG, "Starting dispatcher...");
    view_dispatcher_run(context->view_dispatcher);
}

void switch_to_main_scene(struct ApplicationContext* context) {
    scene_manager_next_scene(context->scene_manager, scene_main);
}

void go_back_to_main_scene(struct ApplicationContext* context) {
    scene_manager_search_and_switch_to_previous_scene(context->scene_manager, scene_main);
}

void send_tick_to_scene(struct ApplicationContext* context) {
    scene_manager_handle_tick_event(context->scene_manager);
}

void go_to_candy_animation(struct ApplicationContext* context) {
    scene_manager_next_scene(context->scene_manager, scene_candy);
}

void go_to_pill_animation(struct ApplicationContext* context) {
    scene_manager_next_scene(context->scene_manager, scene_pill);
}
