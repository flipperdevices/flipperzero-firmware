#include <furi.h>
#include <gui/view_dispatcher.h>

#include "loading_scene.h"
#include "scenes.h"
#include "../../flipper_structs.h"
#include "../../constants.h"

void scene_loading_on_enter(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "scene_loading_on_enter");
    view_dispatcher_switch_to_view(context->view_dispatcher, scene_loading);
}

bool scene_loading_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    FURI_LOG_T(LOG_TAG, "scene_loading_on_event");
    return false;
}

void scene_loading_on_exit(void* ctx) {
    UNUSED(ctx);
    FURI_LOG_T(LOG_TAG, "scene_loading_on_exit");
}
