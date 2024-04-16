#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) NfcPlaylistScene_##id,
typedef enum {
#include "nfc_playlist_scene_config.h"
    NfcPlaylistScene_Count
} NfcPlaylistScene;
#undef ADD_SCENE

extern const SceneManagerHandlers nfc_playlist_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_##name##_scene_on_enter(void*);
#include "nfc_playlist_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_##name##_scene_on_event(void* context, SceneManagerEvent event);
#include "nfc_playlist_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_##name##_scene_on_exit(void* context);
#include "nfc_playlist_scene_config.h"
#undef ADD_SCENE