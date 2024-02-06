#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "reset_scene.h"
#include "scenes.h"
#include "../../constants.h"
#include "../../flipper_structs.h"

static void reset_result_callback(DialogExResult result, void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "reset_result_callback");
    // Handle it in the *_on_event function
    view_dispatcher_send_custom_event(context->view_dispatcher, result);
}

void scene_reset_on_enter(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    DialogEx* dem = context->dialog_ex_module;
    FURI_LOG_T(LOG_TAG, "scene_reset_on_enter");

    dialog_ex_reset(dem);
    dialog_ex_set_header(
        dem,
        "Reset game",
        40,
        5, // x, y position
        AlignLeft,
        AlignTop); // Horizontal, Vertical
    dialog_ex_set_text(
        dem,
        "All progress will be destroyed\n"
        "Do you want to continue?",
        5,
        20, // x, y position
        AlignLeft,
        AlignTop); // Horizontal, Vertical
    dialog_ex_set_context(dem, context);
    dialog_ex_set_result_callback(dem, reset_result_callback);
    dialog_ex_set_left_button_text(dem, "Back");
    dialog_ex_set_right_button_text(dem, "Confirm");

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_reset);
}

bool scene_reset_on_event(void* ctx, SceneManagerEvent event) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_reset_on_event");

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        FURI_LOG_T(LOG_TAG, "scene_reset_on_event called with custom_event %lu", event.event);
        uint32_t index = event.event;

        if(index == DialogExResultRight) {
            struct ThreadsMessage threads_message = {.type = RESET_STATE};
            furi_message_queue_put(
                context->threads_message_queue, &threads_message, FuriWaitForever);
            return true;
        } else if(index == DialogExResultLeft) {
            // Go back
            return scene_manager_previous_scene(context->scene_manager);
        }
        break;
    default:
        // Ignore
        break;
    }
    return false;
}

void scene_reset_on_exit(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_reset_on_exit");
    dialog_ex_reset(context->dialog_ex_module);
}
