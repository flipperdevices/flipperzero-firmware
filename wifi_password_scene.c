#include "wifi_password_scene.h"

void spotify_remote_wifi_password_callback(void* context) {
    SpotifyRemoteApp* app = context;
    scene_manager_handle_custom_event(
        app->scene_manager, SPOTIFY_REMOTE_TEXT_INPUT_SCENE_SAVE_EVENT);
}

// wifi password text input callbacks
void spotify_remote_wifi_password_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;
    bool clear_text = true;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Password");
    text_input_set_result_callback(
        app->text_input,
        spotify_remote_wifi_password_callback,
        app,
        app->wifi_password,
        app->wifi_password_size,
        clear_text);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_INPUT_VIEW);
}

bool spotify_remote_wifi_password_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SPOTIFY_REMOTE_TEXT_INPUT_SCENE_SAVE_EVENT) {
            save_password(app->wifi_password);
            scene_manager_search_and_switch_to_another_scene(
                app->scene_manager, SPOTIFY_REMOTE_CONFIG_SCENE);
            consumed = true;
        }
    }
    return consumed;
}

void spotify_remote_wifi_password_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    text_input_reset(app->text_input);
}