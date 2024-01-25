#pragma once

#include "../spotify_remote.h"

// menu index
typedef enum {
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH,
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG,
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT,
} SpotifyRemoteMainMenuIndex;

// id for menu item events
typedef enum {
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_LAUNCH_EVENT,
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_CONFIG_EVENT,
    SPOTIFY_REMOTE_MAIN_MENU_SCENE_ABOUT_EVENT,
} SpotifyRemoteMainMenuEvent;

void spotify_remote_main_menu_callback(void* context, uint32_t index);
void spotify_remote_main_menu_scene_on_enter(void* context);
bool spotify_remote_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void spotify_remote_main_menu_scene_on_exit(void* context);
