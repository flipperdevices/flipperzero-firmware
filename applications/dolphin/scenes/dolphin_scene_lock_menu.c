#include "../dolphin_i.h"
#include "../views/dolphin_lock_menu_view.h"

void dolphin_scene_lock_menu_callback(DolphinLockMenuEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_lock_menu_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;

    dolphin_lock_menu_set_callback(dolphin->lock_menu, dolphin_scene_lock_menu_callback, dolphin);
    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewLockMenu);
}

const bool dolphin_scene_lock_menu_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinLockMenuEventLock:
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneLocked);
            consumed = true;
            break;

        case DolphinLockMenuEventExit:
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneMain);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

const void dolphin_scene_lock_menu_on_exit(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    dolphin_lock_menu_reset_idx(dolphin->lock_menu);
}
