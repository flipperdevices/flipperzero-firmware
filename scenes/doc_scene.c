#include "doc_scene.h"
#include "doc_i.h"

// Generate scene on_ender handlers declaration
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const doc_on_enter_handlers[])(void* ctx) = {
#include "doc_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const doc_on_event_handlers[])(void* ctx, SceneManagerEvent event) = {
#include "doc_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const doc_on_exit_handlers[])(void* ctx) = {
#include "doc_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers doc_scene_handlers = {
    .on_enter_handlers = doc_on_enter_handlers,
    .on_event_handlers = doc_on_event_handlers,
    .on_exit_handlers = doc_on_exit_handlers,
    .scene_num = DocSceneNum,
};

void doc_scene_callback(void* ctx, uint8_t index) {
    furi_assert(ctx);
    Doc* app = ctx;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}