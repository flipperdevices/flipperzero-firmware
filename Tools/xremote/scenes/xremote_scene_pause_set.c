#include "../xremote.h"
#include "../views/xremote_pause_set.h"

void xremote_scene_pause_set_callback(XRemoteCustomEvent event, void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void xremote_scene_pause_set_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    xremote_pause_set_set_callback(app->xremote_pause_set, xremote_scene_pause_set_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdPauseSet);
}

bool xremote_scene_pause_set_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case XRemoteCustomEventInfoscreenLeft:
            case XRemoteCustomEventInfoscreenRight:
                break;
            case XRemoteCustomEventInfoscreenUp:
            case XRemoteCustomEventInfoscreenDown:
                break;
            case XRemoteCustomEventInfoscreenOk:
                scene_manager_next_scene(app->scene_manager, XRemoteSceneMenu);
                consumed = true;
                break;
            case XRemoteCustomEventPauseSetBack:
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, XRemoteSceneCreateAdd)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
            case XRemoteCustomEventPauseSetOk:
                //cross_remote_add_pause(app->cross_remote, time);
                scene_manager_search_and_switch_to_previous_scene(app->scene_manager, XRemoteSceneCreate);
                consumed = true;
                break;
        }
    }
    return consumed;
}


void xremote_scene_pause_set_on_exit(void* context) {
    UNUSED(context);
}
