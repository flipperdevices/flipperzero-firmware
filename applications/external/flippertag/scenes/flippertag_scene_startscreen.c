#include "../flippertag.h"
#include "../helpers/flippertag_custom_event.h"
#include "../views/flippertag_startscreen.h"

void flippertag_scene_startscreen_callback(FlipperTagCustomEvent event, void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flippertag_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    flippertag_startscreen_set_callback(
        app->flippertag_startscreen, flippertag_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipperTagViewIdStartscreen);
}

bool flippertag_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    FlipperTag* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipperTagCustomEventStartscreenLeft:
            app->team = FlipperTagSolo;
            scene_manager_next_scene(app->scene_manager, FlipperTagSceneScene_1);
            consumed = true;
            break;
        case FlipperTagCustomEventStartscreenRight:
            app->team = FlipperTagTeam1;
            scene_manager_next_scene(app->scene_manager, FlipperTagSceneScene_1);
            consumed = true;
            break;
        case FlipperTagCustomEventStartscreenUp:
        case FlipperTagCustomEventStartscreenDown:
            break;
        case FlipperTagCustomEventStartscreenOk:
            app->team = FlipperTagTeam2;
            scene_manager_next_scene(app->scene_manager, FlipperTagSceneScene_1);
            consumed = true;
            break;
        case FlipperTagCustomEventStartscreenBack:
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

void flippertag_scene_startscreen_on_exit(void* context) {
    FlipperTag* app = context;
    UNUSED(app);
}