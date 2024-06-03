#include "../flipenigma.h"
#include "../helpers/flipenigma_file.h"
#include "../helpers/flipenigma_custom_event.h"
#include "../views/flipenigma_startscreen.h"

void flipenigma_scene_startscreen_callback(FlipEnigmaCustomEvent event, void* context) {
    furi_assert(context);
    FlipEnigma* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipenigma_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    FlipEnigma* app = context;

    // if(flipenigma_has_file(FlipEnigmaFileBoard, NULL, false)) {
    //     if(flipenigma_load_file(app->cipher_text, FlipEnigmaFileBoard, NULL)) {
    //         app->import_game = 1;
    //     }
    // }

    if(app->input_state == FlipEnigmaTextInputActive) {
        // handle text input; this only uses this scene to have
        // a correct stack of scenes
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdTextInput);
    } else {
        // handle default mode; actually use this scene's logic
        flipenigma_startscreen_set_callback(
            app->flipenigma_startscreen, flipenigma_scene_startscreen_callback, app);
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdStartscreen);
    }
}

bool flipenigma_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    FlipEnigma* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipEnigmaCustomEventStartscreenLeft:
        case FlipEnigmaCustomEventStartscreenRight:
            break;
        case FlipEnigmaCustomEventStartscreenUp:
        case FlipEnigmaCustomEventStartscreenDown:
            break;
        case FlipEnigmaCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneMenu);
            consumed = true;
            break;
        case FlipEnigmaCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipEnigmaSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipenigma_scene_startscreen_on_exit(void* context) {
    UNUSED(context);
    // FlipEnigma* app = context;
}