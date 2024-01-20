#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_add_code.h"
#include <furi_hal_cortex.h>


void fcom_add_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomKeyboardView);
}

bool fcom_add_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    return false; //consumed event
}

void fcom_add_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
}


