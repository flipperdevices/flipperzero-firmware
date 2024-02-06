#include <furi.h>
#include <gui/modules/popup.h>
#include <gui/view_dispatcher.h>

#include "pill_scene.h"
#include "scenes.h"
#include "../game_decoder.h"
#include "../../constants.h"
#include "../../flipper_structs.h"

static uint32_t pill_ticks_count;

static void scene_pill_refresh_view(Popup* popup, struct ApplicationContext* context) {
    // Build the view
    popup_reset(popup);
    popup_set_icon(
        popup,
        0,
        2, // x, y
        decode_image(context->game_state));
    popup_set_header(
        popup,
        "Healing",
        72,
        5, // x, y position
        AlignLeft,
        AlignTop); // Horizontal, Vertical
    popup_set_text(popup, "Your pet\nis getting\na pill!", 70, 20, AlignLeft, AlignTop);
}

void scene_pill_on_enter(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    Popup* pp = context->popup_module;
    FURI_LOG_T(LOG_TAG, "scene_pill_on_enter");

    scene_pill_refresh_view(pp, context);

    pill_ticks_count = 0;

    // Start the view
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_pill);
}

bool scene_pill_on_event(void* ctx, SceneManagerEvent event) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;

    switch(event.type) {
    case SceneManagerEventTypeTick:
        FURI_LOG_T(LOG_TAG, "scene_pill_on_event: received tick");
        if(pill_ticks_count >= ANIMATION_TICKS_DURATION) {
            scene_manager_previous_scene(context->scene_manager);
        } else {
            pill_ticks_count++;
            // Refresh the image
            scene_pill_refresh_view(context->popup_module, context);
        }
        return true;
    case SceneManagerEventTypeBack:
        // Don't go back, let's wait for the timeout
        FURI_LOG_T(LOG_TAG, "scene_pill_on_event: pressed back button");
        return true; // It tells to the view dispatcher that this event has been already handled
    default:
        FURI_LOG_T(LOG_TAG, "scene_pill_on_event: unhandled event");
        break;
    }
    return false;
}

void scene_pill_on_exit(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_pill_on_exit");
    popup_reset(context->popup_module);
}
