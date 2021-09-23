#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) FlipperWorldScene##id,
typedef enum {
#include "flipper_world_scene_config.h"
    FlipperWorldSceneNum,
} FlipperWorldScene;
#undef ADD_SCENE

extern const SceneManagerHandlers flipper_world_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "flipper_world_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "flipper_world_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "flipper_world_scene_config.h"
#undef ADD_SCENE
