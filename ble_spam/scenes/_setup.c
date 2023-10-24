#include "_setup.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(name, id) scene_##name##_on_enter,
void (*const scene_on_enter_handlers[])(void*) = {
#include "_scenes.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(name, id) scene_##name##_on_event,
bool (*const scene_on_event_handlers[])(void*, SceneManagerEvent) = {
#include "_scenes.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(name, id) scene_##name##_on_exit,
void (*const scene_on_exit_handlers[])(void*) = {
#include "_scenes.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers scene_handlers = {
    .on_enter_handlers = scene_on_enter_handlers,
    .on_event_handlers = scene_on_event_handlers,
    .on_exit_handlers = scene_on_exit_handlers,
    .scene_num = SceneCOUNT,
};
