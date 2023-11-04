#include <furi.h>
#include <gui/modules/button_panel.h>
#include <gui/view_dispatcher.h>

#include "main_scene.h"
#include "scenes.h"
#include "game_decoder.h"
#include "../flipper_structs.h"
#include "../constants.h"

static void scene_main_refresh_view(ButtonPanel *button_panel, struct GameState *game_state) {
    button_panel_reset(button_panel);

    // Setting up the layout of the view
    button_panel_reserve(button_panel,
                         2, // Number of columns
                         2); // Number of rows
    button_panel_add_icon(button_panel,
                          0, 2,
                          decode_image(game_state));
}

void scene_main_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_enter");

    scene_main_refresh_view(context->button_module, context->game_state);

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_main);
}

bool scene_main_on_event(void *ctx, SceneManagerEvent event) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;

    switch(event.type) {
        case SceneManagerEventTypeBack:
            // Don't go back to loading scene, just exit
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: pressed back button");
            scene_manager_stop(context->scene_manager); // This calls the on_exit function
            return true;
        case SceneManagerEventTypeTick:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received tick");
            // Refresh the image
            scene_main_refresh_view(context->button_module, context->game_state);
            return true;
        case SceneManagerEventTypeCustom:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received button press");
            // Handle button pressed
            // Not handled yet
            return false;
        default:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: unknown event");
            return false;
    }
}

void scene_main_on_exit(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_exit");
    button_panel_reset(context->button_module);
    view_dispatcher_stop(context->view_dispatcher); // Stop the view dispatcher to terminate the GUI
}
