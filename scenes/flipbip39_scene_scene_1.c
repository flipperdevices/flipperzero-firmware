#include "../flipbip39.h"
#include "../helpers/flipbip39_custom_event.h"
#include "../views/flipbip39_scene_1.h"

void flipbip39_scene_1_callback(FlipBip39CustomEvent event, void* context) {
    furi_assert(context);
    FlipBip39* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipbip39_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    FlipBip39* app = context;
    flipbip39_scene_1_set_callback(app->flipbip39_scene_1, flipbip39_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBip39ViewIdScene1);
}

bool flipbip39_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    FlipBip39* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case FlipBip39CustomEventScene1Left:
            case FlipBip39CustomEventScene1Right:
                break;
            case FlipBip39CustomEventScene1Up:
            case FlipBip39CustomEventScene1Down:
                break;
            case FlipBip39CustomEventScene1Back:
                notification_message(app->notification, &sequence_reset_red);
                notification_message(app->notification, &sequence_reset_green);
                notification_message(app->notification, &sequence_reset_blue);
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, FlipBip39SceneMenu)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }
    }
    
    return consumed;
}

void flipbip39_scene_scene_1_on_exit(void* context) {
    FlipBip39* app = context;
    UNUSED(app);
}