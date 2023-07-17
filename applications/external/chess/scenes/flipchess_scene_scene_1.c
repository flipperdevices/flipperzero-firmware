#include "../flipchess.h"
#include "../helpers/flipchess_file.h"
#include "../helpers/flipchess_custom_event.h"
#include "../views/flipchess_scene_1.h"

void flipchess_scene_1_callback(FlipChessCustomEvent event, void* context) {
    furi_assert(context);
    FlipChess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipchess_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    FlipChess* app = context;

    flipchess_scene_1_set_callback(app->flipchess_scene_1, flipchess_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdScene1);
}

bool flipchess_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    FlipChess* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipChessCustomEventScene1Left:
        case FlipChessCustomEventScene1Right:
            break;
        case FlipChessCustomEventScene1Up:
        case FlipChessCustomEventScene1Down:
            break;
        case FlipChessCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipChessSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipchess_scene_scene_1_on_exit(void* context) {
    FlipChess* app = context;

    if(app->import_game == 1 && strlen(app->import_game_text) > 0) {
        flipchess_save_file(app->import_game_text, FlipChessFileBoard, NULL, false, true);
    }
}