#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"
#include "scene_edit.h"

/** collection of all scene on_enter handlers */
void (*const resistors_scene_on_enter_handlers[])(void*) = {
    resistors_main_menu_scene_on_enter,
    resistors_edit_scene_on_enter};

/** collection of all scene on event handlers */
bool (*const resistors_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    resistors_main_menu_scene_on_event,
    resistors_edit_scene_on_event};

/** collection of all scene on exit handlers */
void (*const resistors_scene_on_exit_handlers[])(void*) = {
    resistors_main_menu_scene_on_exit,
    resistors_edit_scene_on_exit};

/** collection of all on_enter, on_event, on_exit handlers */
const SceneManagerHandlers resistors_scene_manager_handlers = {
    .on_enter_handlers = resistors_scene_on_enter_handlers,
    .on_event_handlers = resistors_scene_on_event_handlers,
    .on_exit_handlers = resistors_scene_on_exit_handlers,
    .scene_num = ResistorsSceneCount};

/* callbacks */

/** custom event handler */
bool resistors_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool resistors_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}
