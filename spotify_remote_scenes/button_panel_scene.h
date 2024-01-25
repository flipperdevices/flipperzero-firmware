#pragma once

#include "../spotify_remote.h"

typedef enum {
    PREV,
    NEXT,
    PLAY,
    PAUSE,
    SHUFFLE,
    REPEAT,
} ButtonPanelIndex;

void button_panel_on_select_callback(void* context, uint32_t index);
void spotify_remote_button_panel_scene_on_enter(void* context);
bool spotify_remote_button_panel_scene_on_event(void* context, SceneManagerEvent event);
void spotify_remote_button_panel_scene_on_exit(void* context);
