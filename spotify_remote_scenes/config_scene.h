#pragma once

#include "../spotify_remote.h"

// menu index
typedef enum {
    SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID,
    SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_PASSWORD,
} SpotifyRemoteWifiSubmenuIndex;

// id for menu item events
typedef enum {
    SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_SSID_EVENT,
    SPOTIFY_REMOTE_CONFIG_SCENE_WIFI_PASSWORD_EVENT,
} SpotifyRemoteWifiSubmenuEvent;

void spotify_remote_config_callback(void* context, uint32_t index);
void spotify_remote_config_scene_on_enter(void* context);
bool spotify_remote_config_scene_on_event(void* context, SceneManagerEvent event);
void spotify_remote_config_scene_on_exit(void* context);