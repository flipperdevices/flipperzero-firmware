#include "main_menu_scene.h"

// action taken based on submenu option
void spotify_remote_main_menu_callback(void* context, uint32_t index) {
    SpotifyRemoteApp* app = context;
    switch(index) {
    case SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH:
        app->is_remote_launched = true;
        uart_helper_send(app->uart_helper, "7\n", 2);
        uart_helper_send(app->uart_helper, "7\n", 2);
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH_EVENT);
        break;
    case SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG:
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG_EVENT);
        break;
    case SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT:
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT_EVENT);
        break;
    default:
        break;
    }
}

// main menu scene manager callbacks
void spotify_remote_main_menu_scene_on_enter(void* context) {
    SpotifyRemoteApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Spotify Remote");

    submenu_add_item(
        app->submenu,
        "Launch Remote",
        SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH,
        spotify_remote_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Config",
        SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG,
        spotify_remote_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "About",
        SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT,
        spotify_remote_main_menu_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_SUBMENU_VIEW);
}

bool spotify_remote_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH_EVENT:
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_LOADING_SCENE);
            consumed = true;
            break;
        case SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG_EVENT:
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_WIFI_SSID_SCENE);
            consumed = true;
            break;
        case SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT_EVENT:
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_ABOUT_SCENE);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }

    return consumed;
}

void spotify_remote_main_menu_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    submenu_reset(app->submenu);
}