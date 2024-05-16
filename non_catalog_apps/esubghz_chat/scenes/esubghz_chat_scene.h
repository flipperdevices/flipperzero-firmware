#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) ESubGhzChatScene_##id,
typedef enum {
#include "esubghz_chat_scene_config.h"
    ESubGhzChatScene_MAX
} ESubGhzChatScene;
#undef ADD_SCENE

extern const SceneManagerHandlers esubghz_chat_scene_event_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void scene_on_enter_##name(void*);
#include "esubghz_chat_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool scene_on_event_##name(void* context, SceneManagerEvent event);
#include "esubghz_chat_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void scene_on_exit_##name(void* context);
#include "esubghz_chat_scene_config.h"
#undef ADD_SCENE
