#pragma once

#include "spotify_remote.h"
#include "file_operations.h"

void spotify_remote_wifi_password_callback(void* context);
void spotify_remote_wifi_password_scene_on_enter(void* context);
bool spotify_remote_wifi_password_scene_on_event(void* context, SceneManagerEvent event);
void spotify_remote_wifi_password_scene_on_exit(void* context);