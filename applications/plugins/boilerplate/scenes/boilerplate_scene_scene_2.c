#include "../boilerplate.h"
#include "../helpers/boilerplate_custom_event.h"
#include "../helpers/boilerplate_haptic.h"
#include "../helpers/boilerplate_led.h"
#include "../views/boilerplate_scene_2.h"

void boilerplate_scene_2_callback(BoilerplateCustomEvent event, void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void boilerplate_scene_scene_2_on_enter(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    boilerplate_scene_2_set_callback(app->boilerplate_scene_2, boilerplate_scene_2_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BoilerplateViewIdScene2);
}

bool boilerplate_scene_scene_2_on_event(void* context, SceneManagerEvent event) {
    Boilerplate* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case BoilerplateCustomEventScene2Left:
        case BoilerplateCustomEventScene2Right:
            break;
        case BoilerplateCustomEventScene2Up:
        case BoilerplateCustomEventScene2Down:
            break;
        case BoilerplateCustomEventScene2Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, BoilerplateSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void boilerplate_scene_scene_2_on_exit(void* context) {
    Boilerplate* app = context;
    UNUSED(app);
}
