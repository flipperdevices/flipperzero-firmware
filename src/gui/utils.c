#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include "scenes.h"
#include "utils.h"
#include "../constants.h"

void start_gui_and_block(struct ApplicationContext *context) {
    // Select the first scene to launch
    scene_manager_next_scene(context->scene_manager, scene_loading);

    /* Blocking call until the back button is pressed from the main view */
    FURI_LOG_D(LOG_TAG, "Starting dispatcher...");
    view_dispatcher_run(context->view_dispatcher);
}

void switch_to_main_scene(struct ApplicationContext *context) {
    scene_manager_next_scene(context->scene_manager, scene_main);
}

void send_tick_to_scene(struct ApplicationContext *context) {
    scene_manager_handle_tick_event(context->scene_manager);
}
