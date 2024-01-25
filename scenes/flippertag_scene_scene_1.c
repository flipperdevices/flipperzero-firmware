#include "../flippertag.h"
#include "../helpers/flippertag_custom_event.h"
#include "../views/flippertag_scene_1.h"

void flippertag_scene_1_callback(FlipperTagCustomEvent event, void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flippertag_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    flippertag_scene_1_set_callback(app->flippertag_scene_1, flippertag_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTagViewIdScene1);
}

bool flippertag_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    FlipperTag* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipperTagCustomEventScene1Left:
        case FlipperTagCustomEventScene1Right:
            break;
        case FlipperTagCustomEventScene1Up:
        case FlipperTagCustomEventScene1Down:
            break;
        case FlipperTagCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipperTagSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flippertag_scene_scene_1_on_exit(void* context) {
    FlipperTag* app = context;
    UNUSED(app);
}