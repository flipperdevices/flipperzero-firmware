#include "app_i.h"

#include <furi.h>

#include "flasher/flasher.h"

static void scene_install_flasher_callback(FlasherEvent event, void* context) {
    furi_assert(context);
    App* app = context;

    if(event.type == FlasherEventTypeProgress) {
        progress_set_value(app->progress, event.progress);
    } else if(event.type == FlasherEventTypeSuccess) {
        scene_manager_next_scene(app->scene_manager, SceneSuccess);
    } else if(event.type == FlasherEventTypeError) {
        app->flasher_error = event.error;
        scene_manager_next_scene(app->scene_manager, SceneError);
    }
}

void scene_install_on_enter(void* context) {
    App* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdProgress);

    flasher_set_callback(scene_install_flasher_callback, app);
    flasher_start(furi_string_get_cstr(app->file_path));
}

bool scene_install_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return true;
}

void scene_install_on_exit(void* context) {
    App* app = context;
    progress_reset(app->progress);
}
