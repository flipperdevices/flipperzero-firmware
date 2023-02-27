#include "../color_guess.h"
#include "../helpers/color_guess_custom_event.h"
#include "../views/color_guess_play.h"

void color_guess_play_callback(ColorGuessCustomEvent event, void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void color_guess_scene_play_on_enter(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    color_guess_play_set_callback(app->color_guess_play, color_guess_play_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdPlay);
}

bool color_guess_scene_play_on_event(void* context, SceneManagerEvent event) {
    ColorGuess* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ColorGuessCustomEventPlayLeft:
        case ColorGuessCustomEventPlayRight:
            break;
        case ColorGuessCustomEventPlayUp:
        case ColorGuessCustomEventPlayDown:
            break;
        case ColorGuessCustomEventPlayBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, ColorGuessSceneStart)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void color_guess_scene_play_on_exit(void* context) {
    ColorGuess* app = context;
    UNUSED(app);
}