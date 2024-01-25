#include "wifi_ssid_scene.h"

void spotify_remote_wifi_ssid_callback(void* context) {
    SpotifyRemoteApp* app = context;
    scene_manager_handle_custom_event(
        app->scene_manager, SPOTIFY_REMOTE_TEXT_INPUT_SCENE_SAVE_EVENT);
}

// wifi ssid text input callbacks
void spotify_remote_wifi_ssid_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;

    bool clear_text = true;

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter SSID");
    text_input_set_result_callback(
        app->text_input,
        spotify_remote_wifi_ssid_callback,
        app,
        app->wifi_ssid,
        app->wifi_ssid_size,
        clear_text);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_INPUT_VIEW);
}

bool spotify_remote_wifi_ssid_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;

    // append : character to wifi ssid
    FuriString* wifi_ssid = furi_string_alloc_set_str(app->wifi_ssid);
    furi_string_push_back(wifi_ssid, ':');

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SPOTIFY_REMOTE_TEXT_INPUT_SCENE_SAVE_EVENT) {
            save_ssid(furi_string_get_cstr(wifi_ssid));
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_WIFI_PASSWORD_SCENE);
            consumed = true;
        }
    }

    furi_string_free(wifi_ssid);
    return consumed;
}

void spotify_remote_wifi_ssid_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    text_input_reset(app->text_input);
}