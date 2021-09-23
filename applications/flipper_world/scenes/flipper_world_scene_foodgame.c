#include "../flipper_world_i.h"
#include "../views/flipper_world_foodgame.h"
#include "applications.h"

#define MAIN_VIEW_DEFAULT (0UL)

void flipper_world_scene_foodgame_callback(FlipperFoodGameEvent event, void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    view_dispatcher_send_custom_event(flipper_world->view_dispatcher, event);
}

const void flipper_world_scene_foodgame_on_enter(void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    FlipperFoodGameView* food_game = flipper_world->food_game;

    flipper_foodgame_set_callback(food_game, flipper_world_scene_foodgame_callback, flipper_world);
    osTimerStart(food_game->timer, 60);

    with_view_model(
        food_game->view, (FlipperFoodGameViewModel * model) {
            model->try = 0;
            model->cursor_pos = 0;
            model->lid_pos = 0;
            model->timeout = 0;
            model->current_event = 0;
            model->selected = false;
            flipper_foodgame_reset_loot(model);
            return true;
        });

    view_dispatcher_switch_to_view(flipper_world->view_dispatcher, FlipperWorldFoodGame);
}

const bool flipper_world_scene_foodgame_on_event(void* context, SceneManagerEvent event) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipperFoodGameEventUpdate:
            flipper_foodgame_update_state(flipper_world->food_game);
            consumed = true;
            break;
        case FlipperFoodGameEventExit:
            scene_manager_previous_scene(flipper_world->scene_manager);
            consumed = true;
            break;

        default:
            break;
        }
    }

    return consumed;
}

const void flipper_world_scene_foodgame_on_exit(void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    osTimerStop(flipper_world->food_game->timer);
}
