#include "../dolphin_i.h"
#include "../views/dolphin_debug_view.h"

void dolphin_scene_debug_callback(DolphinDebugEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_debug_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    DolphinDebugView* debug_view = dolphin->debug_view;

    dolphin_debug_set_callback(debug_view, dolphin_scene_debug_callback, dolphin);
    dolphin_debug_get_dolphin_data(debug_view, dolphin->state);
    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewDebug);
}

const bool dolphin_scene_debug_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinDebugEventExit:
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneMain);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

const void dolphin_scene_debug_on_exit(void* context) {
    // Dolphin* dolphin = (Dolphin*)context;
}
