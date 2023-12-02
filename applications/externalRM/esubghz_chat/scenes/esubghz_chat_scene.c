#include "esubghz_chat_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) scene_on_enter_##name,
void (*const esubghz_chat_scene_on_enter_handlers[])(void*) = {
#include "esubghz_chat_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) scene_on_event_##name,
bool (*const esubghz_chat_scene_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "esubghz_chat_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) scene_on_exit_##name,
void (*const esubghz_chat_scene_on_exit_handlers[])(void* context) = {
#include "esubghz_chat_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers esubghz_chat_scene_event_handlers = {
    .on_enter_handlers = esubghz_chat_scene_on_enter_handlers,
    .on_event_handlers = esubghz_chat_scene_on_event_handlers,
    .on_exit_handlers = esubghz_chat_scene_on_exit_handlers,
    .scene_num = ESubGhzChatScene_MAX,
};
