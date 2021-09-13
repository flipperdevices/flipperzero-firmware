#include "../dolphin_i.h"
#include "../views/dolphin_hw_mismatch_view.h"

void dolphin_scene_hw_mismatch_callback(DolphinHwMismatchEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_hw_mismatch_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;

    dolphin_hw_mismatch_set_callback(
        dolphin->hw_mismatch_view, dolphin_scene_hw_mismatch_callback, dolphin);
    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewHwMismatch);
}

const bool dolphin_scene_hw_mismatch_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinHwMismatchEventExit:
            scene_manager_previous_scene(dolphin->scene_manager);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

const void dolphin_scene_hw_mismatch_on_exit(void* context) {
    // Dolphin* dolphin = (Dolphin*)context;
}
