

#include "quac.h"
#include "scenes.h"
#include "scene_items.h"

// define handler callbacks - order must match appScenes enum!
void (*const app_on_enter_handlers[])(void* context) = {scene_items_on_enter};
bool (*const app_on_event_handlers[])(void* context, SceneManagerEvent event) = {
    scene_items_on_event,
};
void (*const app_on_exit_handlers[])(void* context) = {scene_items_on_exit};

const SceneManagerHandlers app_scene_handlers = {
    .on_enter_handlers = app_on_enter_handlers,
    .on_event_handlers = app_on_event_handlers,
    .on_exit_handlers = app_on_exit_handlers,
    .scene_num = SR_Scene_count};

bool app_scene_custom_callback(void* context, uint32_t custom_event_id) {
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event_id);
}
bool app_back_event_callback(void* context) {
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}
