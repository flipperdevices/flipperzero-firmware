

#include "quac.h"
#include "scenes.h"
#include "scene_items.h"
#include "scene_settings.h"
#include "scene_action_settings.h"
#include "scene_action_rename.h"
#include "scene_action_create_group.h"
#include "scene_action_ir_list.h"
#include "scene_about.h"

// define handler callbacks - order must match appScenes enum!
void (*const app_on_enter_handlers[])(void* context) = {
    scene_items_on_enter,
    scene_settings_on_enter,
    scene_action_settings_on_enter,
    scene_action_rename_on_enter,
    scene_action_create_group_on_enter,
    scene_action_ir_list_on_enter,
    scene_about_on_enter,
};
bool (*const app_on_event_handlers[])(void* context, SceneManagerEvent event) = {
    scene_items_on_event,
    scene_settings_on_event,
    scene_action_settings_on_event,
    scene_action_rename_on_event,
    scene_action_create_group_on_event,
    scene_action_ir_list_on_event,
    scene_about_on_event,
};
void (*const app_on_exit_handlers[])(void* context) = {
    scene_items_on_exit,
    scene_settings_on_exit,
    scene_action_settings_on_exit,
    scene_action_rename_on_exit,
    scene_action_create_group_on_exit,
    scene_action_ir_list_on_exit,
    scene_about_on_exit,
};

const SceneManagerHandlers app_scene_handlers = {
    .on_enter_handlers = app_on_enter_handlers,
    .on_event_handlers = app_on_event_handlers,
    .on_exit_handlers = app_on_exit_handlers,
    .scene_num = QScene_count};

bool app_scene_custom_callback(void* context, uint32_t custom_event_id) {
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event_id);
}
bool app_back_event_callback(void* context) {
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}
