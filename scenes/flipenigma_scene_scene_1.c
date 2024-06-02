#include "../flipenigma.h"
#include "../helpers/flipenigma_file.h"
#include "../helpers/flipenigma_custom_event.h"
#include "../views/flipenigma_scene_1.h"

void flipenigma_scene_1_callback(FlipEnigmaCustomEvent event, void* context) {
    furi_assert(context);
    FlipEnigma* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipenigma_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    FlipEnigma* app = context;

    flipenigma_scene_1_set_callback(app->flipenigma_scene_1, flipenigma_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdScene1);
}

bool flipenigma_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    FlipEnigma* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipEnigmaCustomEventScene1Left:
        case FlipEnigmaCustomEventScene1Right:
            break;
        case FlipEnigmaCustomEventScene1Up:
        case FlipEnigmaCustomEventScene1Down:
            break;
        case FlipEnigmaCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipEnigmaSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipenigma_scene_scene_1_on_exit(void* context) {
    FlipEnigma* app = context;

    if(app->import_game == 1 && strlen(app->import_game_text) > 0) {
        flipenigma_save_file(app->import_game_text, FlipEnigmaFileBoard, NULL, false, true);
    }
}