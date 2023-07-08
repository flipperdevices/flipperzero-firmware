#include "../color_guess.h"

void color_guess_scene_startscreen_callback(ColorGuessCustomEvent event, void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void color_guess_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    color_guess_startscreen_set_callback(
        app->color_guess_startscreen, color_guess_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdStartscreen);
}

bool color_guess_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    ColorGuess* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ColorGuessCustomEventStartscreenLeft:
        case ColorGuessCustomEventStartscreenRight:
            break;
        case ColorGuessCustomEventStartscreenUp:
        case ColorGuessCustomEventStartscreenDown:
            break;
        case ColorGuessCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, ColorGuessSceneMenu);
            consumed = true;
            break;
        case ColorGuessCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, ColorGuessSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void color_guess_scene_startscreen_on_exit(void* context) {
    ColorGuess* app = context;
    UNUSED(app);
}