#include <furi.h>

#include "settings_scene.h"
#include "../flipper_structs.h"
#include "../constants.h"

void scene_settings_on_enter(void *ctx) {
    UNUSED(ctx);
    FURI_LOG_T(LOG_TAG, "scene_settings_on_enter");
}

bool scene_settings_on_event(void *ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    FURI_LOG_T(LOG_TAG, "scene_settings_on_event");
    return false;
}

void scene_settings_on_exit(void *ctx) {
    UNUSED(ctx);
    FURI_LOG_T(LOG_TAG, "scene_settings_on_exit");
}
