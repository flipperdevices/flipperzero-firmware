#include "../can_lin_tools_app_i.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const can_lin_tools_scene_on_enter_handlers[])(void*) = {
#include "can_lin_tools_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const can_lin_tools_scene_on_event_handlers[])(void* context, SceneManagerEvent event) =
    {
#include "can_lin_tools_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const can_lin_tools_scene_on_exit_handlers[])(void* context) = {
#include "can_lin_tools_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers can_lin_tools_scene_handlers = {
    .on_enter_handlers = can_lin_tools_scene_on_enter_handlers,
    .on_event_handlers = can_lin_tools_scene_on_event_handlers,
    .on_exit_handlers = can_lin_tools_scene_on_exit_handlers,
    .scene_num = CanLinToolsSceneNum,
};
