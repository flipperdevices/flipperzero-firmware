#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) Evil_PortalScene##id,
typedef enum {
#include "evil_portal_scene_config.h"
    Evil_PortalSceneNum,
} Evil_PortalScene;
#undef ADD_SCENE

extern const SceneManagerHandlers evil_portal_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "evil_portal_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "evil_portal_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "evil_portal_scene_config.h"
#undef ADD_SCENE
