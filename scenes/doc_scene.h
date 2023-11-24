#pragma once

#include <gui/scene_manager.h>

#define ADD_SCENE(prefix, name, id) DocScene##id,
typedef enum {
#include "doc_scene_config.h"
    DocSceneNum,
} DocScene;
#undef ADD_SCENE

extern const SceneManagerHandlers doc_scene_handlers;

// Generate scene on_ender handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void* ctx);
#include "doc_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* ctx, SceneManagerEvent event);
#include "doc_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* ctx);
#include "doc_scene_config.h"
#undef ADD_SCENE

void doc_scene_callback(void* ctx, uint8_t index);