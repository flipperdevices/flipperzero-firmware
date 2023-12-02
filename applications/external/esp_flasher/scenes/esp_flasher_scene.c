#include "esp_flasher_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const esp_flasher_scene_on_enter_handlers[])(void*) = {
#include "esp_flasher_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const esp_flasher_scene_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "esp_flasher_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const esp_flasher_scene_on_exit_handlers[])(void* context) = {
#include "esp_flasher_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers esp_flasher_scene_handlers = {
    .on_enter_handlers = esp_flasher_scene_on_enter_handlers,
    .on_event_handlers = esp_flasher_scene_on_event_handlers,
    .on_exit_handlers = esp_flasher_scene_on_exit_handlers,
    .scene_num = EspFlasherSceneNum,
};
