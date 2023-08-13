#include "text_viewer_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const text_viewer_on_enter_handlers[])(void*) = {
#include "text_viewer_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const text_viewer_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "text_viewer_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const text_viewer_on_exit_handlers[])(void* context) = {
#include "text_viewer_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers text_viewer_scene_handlers = {
    .on_enter_handlers = text_viewer_on_enter_handlers,
    .on_event_handlers = text_viewer_on_event_handlers,
    .on_exit_handlers = text_viewer_on_exit_handlers,
    .scene_num = TextViewerSceneNum,
};
