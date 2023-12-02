#include "color_guess_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const color_guess_on_enter_handlers[])(void*) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const color_guess_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const color_guess_on_exit_handlers[])(void* context) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers color_guess_scene_handlers = {
    .on_enter_handlers = color_guess_on_enter_handlers,
    .on_event_handlers = color_guess_on_event_handlers,
    .on_exit_handlers = color_guess_on_exit_handlers,
    .scene_num = ColorGuessSceneNum,
};
