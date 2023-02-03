#include "../color_guess.h"

void color_guess_scene_play_on_enter(void* context) {
    ColorGuess* app = context;
    //UNUSED(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdPlay);
}

bool color_guess_scene_play_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void color_guess_scene_play_on_exit(void* context) {
    UNUSED(context);
    
}