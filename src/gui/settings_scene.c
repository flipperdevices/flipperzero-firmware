#include <furi.h>
#include <gui/modules/variable_item_list.h>

#include "settings_scene.h"
#include "scenes.h"
#include "../flipper_structs.h"
#include "../constants.h"

enum ButtonIndexes {
    reset_game,
    about
};

static void settings_enter_callback(void *ctx, uint32_t index) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "settings_enter_callback with index %lu", index);
    // Handle it in the *_on_event function
    view_dispatcher_send_custom_event(context->view_dispatcher, index);
}

void scene_settings_on_enter(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    VariableItemList* vil = context->variable_item_list_module;
    FURI_LOG_T(LOG_TAG, "scene_settings_on_enter");

    variable_item_list_reset(vil);
    variable_item_list_add(vil, "Reset game", 0, NULL, NULL); // Index 0 (reset_game)
    variable_item_list_add(vil,"About", 0, NULL, NULL); // Index 1 (about)

    variable_item_list_set_enter_callback(vil, settings_enter_callback, context);

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_settings);
}

bool scene_settings_on_event(void *ctx, SceneManagerEvent event) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_settings_on_event");

    switch(event.type) {
        case SceneManagerEventTypeCustom:
            FURI_LOG_T(LOG_TAG, "scene_settings_on_event called with custom_event %lu", event.event);
            uint32_t index = event.event;

            if (index == reset_game) {
                scene_manager_next_scene(context->scene_manager, scene_reset);
            } else if (index == about) {
                scene_manager_next_scene(context->scene_manager, scene_about);
            }
            return true; // Let's tell we handled the event
        default:
            // Ignore
            break;
    }
    return false;
}

void scene_settings_on_exit(void *ctx) {
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    FURI_LOG_T(LOG_TAG, "scene_settings_on_exit");
    variable_item_list_reset(context->variable_item_list_module);
}
