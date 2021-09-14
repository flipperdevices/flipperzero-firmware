#include "../dolphin_i.h"
#include "../views/dolphin_first_start_view.h"
#include "../helpers/dolphin_state.h"

void dolphin_scene_first_start_callback(DolphinFirstStartEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_first_start_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    DolphinFirstStartView* first_start_view = dolphin->first_start_view;

    dolphin_first_start_set_callback(
        first_start_view, dolphin_scene_first_start_callback, dolphin);

    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewFirstStart);
}

const bool dolphin_scene_first_start_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinFirstStartCompleted:
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneMain);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

const void dolphin_scene_first_start_on_exit(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    dolphin_state_save(dolphin->state);
}
