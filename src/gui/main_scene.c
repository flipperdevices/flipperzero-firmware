#include <furi.h>
#include <gui/modules/button_panel.h>
#include <gui/view_dispatcher.h>

#include "main_scene.h"
#include "scenes.h"
#include "game_decoder.h"
#include "matagotchi_icons.h"
#include "../flipper_structs.h"
#include "../constants.h"

enum ButtonIndex {
    settings_button,
    candy_button
};

static void main_button_pressed_callback(void *ctx, uint32_t index) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "main_button_pressed_callback with index %lu", index);
    // Handle it in the *_on_event function
    view_dispatcher_send_custom_event(context->view_dispatcher, index);
}

static void scene_main_refresh_view(ButtonPanel *button_panel, struct ApplicationContext *context) {
    button_panel_reset(button_panel);

    // Setting up the layout of the view
    button_panel_reserve(button_panel,
                         2, // Number of columns
                         2); // Number of rows
    button_panel_add_icon(button_panel,
                          0, 2,
                          decode_image(context->game_state));
    button_panel_add_item(button_panel,
                          settings_button, // Index
                          0, 0, // Location in the matrix declared above (x, y)
                          70, 6, // Coordinates where to draw the icon (x, y)
                          &I_settings_icon_20x20, // Icon
                          &I_settings_icon_hover_20x20, // Icon when cursor over it
                          main_button_pressed_callback,
                          context);
    button_panel_add_item(button_panel,
                          candy_button, // Index
                          1, 0, // Location in the matrix declared above (x, y)
                          102, 6, // Coordinates where to draw the icon (x, y)
                          &I_candy_icon_20x20, // Icon
                          &I_candy_icon_hover_20x20, // Icon when cursor over it
                          main_button_pressed_callback,
                          context);
}

void scene_main_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_enter");

    scene_main_refresh_view(context->button_module, context);

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
            view_dispatcher_stop(context->view_dispatcher); // Stop the view dispatcher to terminate the GUI
            return true;
        case SceneManagerEventTypeTick:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received tick");
            // Refresh the image
            scene_main_refresh_view(context->button_module, context);
            return true;
        case SceneManagerEventTypeCustom:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: received button press %lu", event.event);
            uint32_t index = event.event;

            // Handle button pressed
            if (index == settings_button) {
                scene_manager_next_scene(context->scene_manager, scene_settings);
            } else if (index == candy_button) {
                // TODO
            }
            return true; // Let's tell we handled the event
        default:
            FURI_LOG_T(LOG_TAG, "scene_main_on_event: unhandled event");
            break;
    }
    return false;
}

void scene_main_on_exit(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_main_on_exit");
    button_panel_reset(context->button_module);
}
