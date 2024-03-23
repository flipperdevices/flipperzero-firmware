#include "main_menu_scene.h"

/**
 * Action taken based on submenu option
 * 
 * @param context the app object
 * @param index index of submenu option
*/
void spotify_remote_main_menu_callback(void* context, uint32_t index) {
    SpotifyRemoteApp* app = context;
    switch(index) {
    case SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH:
        app->is_remote_launched = true;
        uart_helper_send(app->uart_helper, "7\n", 2);
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH_EVENT);
        break;
    case SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT:
        scene_manager_handle_custom_event(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT_EVENT);
        break;
    default:
        break;
    }
}

/**
 * On enter handler for main menu scene
 * 
 * @param context the app object
*/
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
        "About",
        SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT,
        spotify_remote_main_menu_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, SPOTIFY_REMOTE_SUBMENU_VIEW);
}

/**
 * On event handler for main menu scene
 * 
 * @param context app struct
 * @param event event to handle 
*/
bool spotify_remote_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    SpotifyRemoteApp* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH_EVENT:
            scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_CONNECTING_SCENE);
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

/**
 * On exit handler for main menu scene
 * 
 * @param context - the app object 
*/
void spotify_remote_main_menu_scene_on_exit(void* context) {
    SpotifyRemoteApp* app = context;
    submenu_reset(app->submenu);
}