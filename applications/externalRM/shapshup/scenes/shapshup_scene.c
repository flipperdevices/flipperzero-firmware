#include "shapshup_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const shapshup_on_enter_handlers[])(void*) = {
#include "shapshup_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const shapshup_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "shapshup_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const shapshup_on_exit_handlers[])(void* context) = {
#include "shapshup_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers shapshup_scene_handlers = {
    .on_enter_handlers = shapshup_on_enter_handlers,
    .on_event_handlers = shapshup_on_event_handlers,
    .on_exit_handlers = shapshup_on_exit_handlers,
    .scene_num = ShapshupSceneNum,
};
