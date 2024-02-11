#include <furi.h>
#include <gui/modules/popup.h>
#include <gui/view_dispatcher.h>

#include "status_scene.h"
#include "scenes.h"
#include "matagotchi_icons.h"
#include "../game_decoder.h"
#include "../../constants.h"
#include "../../flipper_structs.h"
#include "../../state_management.h"

#define MAX_TEXT_LENGTH 128
static char* stats_text = NULL; // Dynamically allocated, only lives while this scene is active

static void scene_status_refresh_view(Popup* popup, struct ApplicationContext* context) {
    // Build the status string
    get_state_str(context->game_state, stats_text, MAX_TEXT_LENGTH);

    // Build the view
    popup_reset(popup);
    popup_set_icon(
        popup,
        0,
        2, // x, y
        decode_image(context->game_state));
    popup_set_header(
        popup,
        "Stats",
        85,
        5, // x, y position
        AlignLeft,
        AlignTop); // Horizontal, Vertical
    popup_set_text(popup, stats_text, 64, 20, AlignLeft, AlignTop);
}

void scene_status_on_enter(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_status_on_enter");

    /* Allocate memory for the string we are going to show */
    stats_text = malloc(sizeof(char) * MAX_TEXT_LENGTH);
    furi_assert(stats_text);

    scene_status_refresh_view(context->popup_module, context);

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_status);
}

bool scene_status_on_event(void* ctx, SceneManagerEvent event) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;

    switch(event.type) {
    case SceneManagerEventTypeTick:
        FURI_LOG_T(LOG_TAG, "scene_status_on_event: received tick");
        // Refresh the image
        scene_status_refresh_view(context->popup_module, context);
        return true;
    default:
        FURI_LOG_T(LOG_TAG, "scene_status_on_event: unhandled event");
        break;
    }
    return false;
}

void scene_status_on_exit(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_status_on_exit");
    free(stats_text);
    popup_reset(context->popup_module);
}
