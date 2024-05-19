#include "../meal_pager_i.h"
#include "../helpers/meal_pager_custom_event.h"
#include "../views/meal_pager_startscreen.h"

void meal_pager_scene_startscreen_callback(Meal_PagerCustomEvent event, void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void meal_pager_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    meal_pager_startscreen_set_callback(
        app->meal_pager_startscreen, meal_pager_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, Meal_PagerViewIdStartscreen);
}

bool meal_pager_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    Meal_Pager* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case Meal_PagerCustomEventStartscreenLeft:
        case Meal_PagerCustomEventStartscreenRight:
            break;
        case Meal_PagerCustomEventStartscreenUp:
        case Meal_PagerCustomEventStartscreenDown:
            break;
        case Meal_PagerCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, Meal_PagerSceneMenu);
            consumed = true;
            break;
        case Meal_PagerCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, Meal_PagerSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void meal_pager_scene_startscreen_on_exit(void* context) {
    Meal_Pager* app = context;
    UNUSED(app);
}