#include "../dolphin_i.h"
#include "../views/dolphin_locked_view.h"

void dolphin_scene_locked_callback(DolphinLockedEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_locked_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    DolphinLockedView* locked_view = dolphin->locked_view;

    dolphin_locked_set_callback(locked_view, dolphin_scene_locked_callback, dolphin);
    dolphin_locked_reset_door_pos(locked_view);
    dolphin_locked_update_hint_timeout(locked_view);

    view_port_enabled_set(dolphin->lock_viewport, true);
    osTimerStart(locked_view->timer, 63);

    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewLocked);
}

const bool dolphin_scene_locked_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinLockedEventUnlock:
            scene_manager_set_scene_state(
                dolphin->scene_manager, DolphinSceneMain, DolphinMainEventUnlocked);
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneMain);
            consumed = true;
            break;
        case DolphinLockedEventUpdate:
            dolphin_locked_trigger_redraw(dolphin->locked_view);
            consumed = true;
        default:
            break;
        }
    }

    return consumed;
}

const void dolphin_scene_locked_on_exit(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    DolphinLockedView* locked_view = dolphin->locked_view;
    dolphin_locked_reset_counter(dolphin->locked_view);
    osTimerStop(locked_view->timer);
}
