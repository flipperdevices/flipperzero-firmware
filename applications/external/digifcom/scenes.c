#include "flipper.h"
#include "app_state.h"
#include "scenes.h"

/** collection of all scene on_enter handlers */
void (*const fcom_scene_on_enter_handlers[])(void*) = {
    fcom_main_menu_scene_on_enter,
    fcom_listen_menu_scene_on_enter,
    fcom_read_code_scene_on_enter,
    fcom_select_code_scene_on_enter,
    fcom_send_code_scene_on_enter,
    fcom_add_code_scene_on_enter,
    fcom_serial_scene_on_enter,
    fcom_save_code_scene_on_enter};

/** collection of all scene on event handlers */
bool (*const fcom_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    fcom_main_menu_scene_on_event,
    fcom_listen_menu_scene_on_event,
    fcom_read_code_scene_on_event,
    fcom_select_code_scene_on_event,
    fcom_send_code_scene_on_event,
    fcom_add_code_scene_on_event,
    fcom_serial_scene_on_event,
    fcom_save_code_scene_on_event};

/** collection of all scene on exit handlers */
void (*const fcom_scene_on_exit_handlers[])(void*) = {
    fcom_main_menu_scene_on_exit,
    fcom_listen_menu_scene_on_exit,
    fcom_read_code_scene_on_exit,
    fcom_select_code_scene_on_exit,
    fcom_send_code_scene_on_exit,
    fcom_add_code_scene_on_exit,
    fcom_serial_scene_on_exit,
    fcom_save_code_scene_on_exit};

/** collection of all on_enter, on_event, on_exit handlers */
const SceneManagerHandlers fcom_scene_manager_handlers = {
    .on_enter_handlers = fcom_scene_on_enter_handlers,
    .on_event_handlers = fcom_scene_on_event_handlers,
    .on_exit_handlers = fcom_scene_on_exit_handlers,
    .scene_num = FcomSceneCount};

/* callbacks */

/** custom event handler */
bool fcom_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool fcom_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}
