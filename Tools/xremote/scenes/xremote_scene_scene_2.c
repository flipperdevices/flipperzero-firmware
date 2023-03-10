#include "../xremote.h"
#include "../helpers/xremote_custom_event.h"
#include "../helpers/xremote_haptic.h"
#include "../helpers/xremote_led.h"
#include "../views/xremote_scene_2.h"

void xremote_scene_2_callback(XRemoteCustomEvent event, void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void xremote_scene_scene_2_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    xremote_scene_2_set_callback(app->xremote_scene_2, xremote_scene_2_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdScene2);
}

bool xremote_scene_scene_2_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case XRemoteCustomEventScene2Left:
            case XRemoteCustomEventScene2Right:
                break;
            case XRemoteCustomEventScene2Up:
            case XRemoteCustomEventScene2Down:
                break;
            case XRemoteCustomEventScene2Back:
                notification_message(app->notification, &sequence_reset_red);
                notification_message(app->notification, &sequence_reset_green);
                notification_message(app->notification, &sequence_reset_blue);
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, XRemoteSceneMenu)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }
    }

    return consumed;
}

void xremote_scene_scene_2_on_exit(void* context) {
    XRemote* app = context;
    UNUSED(app);
}

