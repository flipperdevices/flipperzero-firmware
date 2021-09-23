#include "../flipper_world_i.h"
#include "../views/flipper_world_main.h"
#include "applications.h"

#define MAIN_VIEW_DEFAULT (0UL)

void flipper_world_scene_main_callback(FlipperMainEvent event, void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    view_dispatcher_send_custom_event(flipper_world->view_dispatcher, event);
}

const void flipper_world_scene_main_on_enter(void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    FlipperMainView* main_view = flipper_world->main_view;

    flipper_main_set_callback(main_view, flipper_world_scene_main_callback, flipper_world);
    osTimerStart(main_view->timer, 40);

    view_dispatcher_switch_to_view(flipper_world->view_dispatcher, FlipperWorldViewMain);
}

const bool flipper_world_scene_main_on_event(void* context, SceneManagerEvent event) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipperMainEventUpdate:
            flipper_world_tick_handler(flipper_world->main_view);
            consumed = true;
            break;
        case FlipperMainEventStartFoodGame:
            scene_manager_next_scene(flipper_world->scene_manager, FlipperWorldFoodGame);
            consumed = true;
            break;
        case FlipperMainEventStartPassport:
            scene_manager_next_scene(flipper_world->scene_manager, FlipperWorldPassport);
            consumed = true;
            break;
        case FlipperMainEventExit:
            view_dispatcher_stop(flipper_world->view_dispatcher);
            consumed = true;
            break;

        default:
            break;
        }
    }

    return consumed;
}

const void flipper_world_scene_main_on_exit(void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    FlipperMainView* main_view = flipper_world->main_view;

    osTimerStop(main_view->timer);
}
