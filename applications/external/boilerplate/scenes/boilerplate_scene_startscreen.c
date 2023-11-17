#include "../boilerplate.h"
#include "../helpers/boilerplate_custom_event.h"
#include "../views/boilerplate_startscreen.h"

void boilerplate_scene_startscreen_callback(BoilerplateCustomEvent event, void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void boilerplate_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    boilerplate_startscreen_set_callback(
        app->boilerplate_startscreen, boilerplate_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BoilerplateViewIdStartscreen);
}

bool boilerplate_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    Boilerplate* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case BoilerplateCustomEventStartscreenLeft:
        case BoilerplateCustomEventStartscreenRight:
            break;
        case BoilerplateCustomEventStartscreenUp:
        case BoilerplateCustomEventStartscreenDown:
            break;
        case BoilerplateCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, BoilerplateSceneMenu);
            consumed = true;
            break;
        case BoilerplateCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, BoilerplateSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void boilerplate_scene_startscreen_on_exit(void* context) {
    Boilerplate* app = context;
    UNUSED(app);
}