#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(name, id) Scene##id,
typedef enum {
#include "_scenes.h"
    SceneCOUNT,
} Scene;
#undef ADD_SCENE

extern const SceneManagerHandlers scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(name, id) void scene_##name##_on_enter(void*);
#include "_scenes.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(name, id) bool scene_##name##_on_event(void*, SceneManagerEvent);
#include "_scenes.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(name, id) void scene_##name##_on_exit(void*);
#include "_scenes.h"
#undef ADD_SCENE
