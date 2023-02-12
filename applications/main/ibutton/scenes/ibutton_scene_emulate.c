#include "../ibutton_i.h"
#include <core/log.h>
#include <toolbox/path.h>

#define EMULATE_TIMEOUT_TICKS 10

static void ibutton_scene_emulate_callback(void* context, bool emulated) {
    iButton* ibutton = context;
    if(emulated) {
        view_dispatcher_send_custom_event(
            ibutton->view_dispatcher, iButtonCustomEventWorkerEmulated);
    }
}

void ibutton_scene_emulate_on_enter(void* context) {
    iButton* ibutton = context;
    iButtonKey* key = ibutton->key;

    ibutton_worker_emulate_set_callback(
        ibutton->key_worker, ibutton_scene_emulate_callback, ibutton);
    ibutton_worker_emulate_start(ibutton->key_worker, key);

    ibutton_notification_message(ibutton, iButtonNotificationMessageEmulateStart);
    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewWidget);
}

bool ibutton_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        uint32_t cnt = scene_manager_get_scene_state(ibutton->scene_manager, iButtonSceneEmulate);
        if(cnt > 0) {
            cnt--;
            if(cnt == 0) {
                ibutton_notification_message(ibutton, iButtonNotificationMessageEmulateBlink);
            }
            scene_manager_set_scene_state(ibutton->scene_manager, iButtonSceneEmulate, cnt);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == iButtonCustomEventWorkerEmulated) {
            if(scene_manager_get_scene_state(ibutton->scene_manager, iButtonSceneEmulate) == 0) {
                ibutton_notification_message(ibutton, iButtonNotificationMessageYellowBlink);
            }
            scene_manager_set_scene_state(
                ibutton->scene_manager, iButtonSceneEmulate, EMULATE_TIMEOUT_TICKS);
        }
    }

    return consumed;
}

void ibutton_scene_emulate_on_exit(void* context) {
    iButton* ibutton = context;
    ibutton_worker_stop(ibutton->key_worker);
    widget_reset(ibutton->widget);
    ibutton_notification_message(ibutton, iButtonNotificationMessageBlinkStop);
}
