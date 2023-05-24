#include "../flipbip.h"
#include "../helpers/flipbip_custom_event.h"
#include "../views/flipbip_startscreen.h"

void flipbip_scene_startscreen_callback(FlipBipCustomEvent event, void* context) {
    furi_assert(context);
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipbip_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    flipbip_startscreen_set_callback(
        app->flipbip_startscreen, flipbip_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdStartscreen);
}

bool flipbip_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipBipCustomEventStartscreenLeft:
        case FlipBipCustomEventStartscreenRight:
            break;
        case FlipBipCustomEventStartscreenUp:
        case FlipBipCustomEventStartscreenDown:
            break;
        case FlipBipCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, FlipBipSceneMenu);
            consumed = true;
            break;
        case FlipBipCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipBipSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipbip_scene_startscreen_on_exit(void* context) {
    FlipBip* app = context;
    UNUSED(app);
}