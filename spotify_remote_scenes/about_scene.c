#include "about_scene.h"

/**
 * On enter handler for the about scene
 * 
 * @param context the app object
*/
void spotify_remote_about_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;
    text_box_reset(app->text_box);

    text_box_set_font(app->text_box, TextBoxFontText);
    text_box_set_focus(app->text_box, TextBoxFocusStart);
    text_box_set_text(
        app->text_box,
        "Written by @benjamindrussell. For instructions, visit https://github.com/benjamindrussell/flipper-spotify-remote");

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_BOX_VIEW);
}

/**
 * On event handler for the about scene
 * 
 * @param context the app object
 * @param event the event to handle
*/
bool spotify_remote_about_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

/**
 * On exit handler for the about scene
 * 
 * @param context the app object
*/
void spotify_remote_about_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    text_box_reset(app->text_box);
}