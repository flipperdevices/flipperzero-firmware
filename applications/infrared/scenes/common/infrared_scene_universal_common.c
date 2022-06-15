#include "../../infrared_i.h"

#include <dolphin/dolphin.h>

void infrared_scene_universal_common_item_callback(void* context, uint32_t index) {
    Infrared* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

static void infrared_scene_universal_common_progress_back_callback(void* context) {
    Infrared* infrared = context;
    UNUSED(infrared);
}

static void infrared_scene_universal_common_show_popup(Infrared* infrared, uint32_t record_count) {
    ViewStack* view_stack = infrared->view_stack;
    InfraredProgressView* progress = infrared->progress;
    infrared_progress_view_set_progress_total(progress, record_count);
    infrared_progress_view_set_back_callback(
        progress, infrared_scene_universal_common_progress_back_callback, infrared);
    view_stack_add_view(view_stack, infrared_progress_view_get_view(progress));
}

static void infrared_scene_universal_common_hide_popup(Infrared* infrared) {
    ViewStack* view_stack = infrared->view_stack;
    InfraredProgressView* progress = infrared->progress;
    view_stack_remove_view(view_stack, infrared_progress_view_get_view(progress));
}

void infrared_scene_universal_common_on_enter(void* context) {
    Infrared* infrared = context;
    UNUSED(infrared);
}

bool infrared_scene_universal_common_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    InfraredBruteForce* brute_force = infrared->brute_force;
    bool consumed = false;

    if(infrared_brute_force_is_started(brute_force)) {
        if(event.type == SceneManagerEventTypeTick) {
            infrared_play_notification_message(infrared, InfraredNotificationMessageBlinkSend);
            consumed = true;
        } else if(event.type == SceneManagerEventTypeBack) {
            infrared_brute_force_stop(brute_force);
            infrared_scene_universal_common_hide_popup(infrared);
            consumed = true;
        }
    } else {
        if(event.type == SceneManagerEventTypeBack) {
            scene_manager_previous_scene(scene_manager);
            consumed = true;
        } else if(event.type == SceneManagerEventTypeCustom) {
            uint32_t record_count;
            if(infrared_brute_force_start(brute_force, event.event, &record_count)) {
                DOLPHIN_DEED(DolphinDeedIrBruteForce);
                infrared_scene_universal_common_show_popup(infrared, record_count);
                infrared_play_notification_message(infrared, InfraredNotificationMessageBlinkSend);
            } else {
                scene_manager_previous_scene(scene_manager);
            }
            consumed = true;
        }
    }

    return consumed;
}

void infrared_scene_universal_common_on_exit(void* context) {
    Infrared* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    button_panel_reset(button_panel);
}
