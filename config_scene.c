#include "config_scene.h"

void spotify_remote_config_callback(void* context, uint32_t index) {
    SpotifyRemoteApp* app = context;
    switch(index) {
    case SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID:
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID_EVENT);
        break;
    default:
        break;
    }
}

// config submenu scene callbacks
void spotify_remote_config_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "WiFi Config");
    app->is_wifi_config_launched = false;

    submenu_add_item(
        app->submenu,
        "WiFi Config",
        SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID,
        spotify_remote_config_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_SUBMENU_VIEW);
}

bool spotify_remote_config_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID_EVENT:
            app->is_wifi_config_launched = true;
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_WIFI_SSID_SCENE);
            break;
        }
    default:
        break;
    }

    return consumed;
}

void spotify_remote_config_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    submenu_reset(app->submenu);
}
