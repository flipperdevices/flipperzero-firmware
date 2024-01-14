#include "../mfc_editor_app_i.h"

void mfc_editor_scene_start_on_enter(void* context) {
    MfcEditorApp* instance = context;
    UNUSED(instance);
    UNUSED(context);
}

bool mfc_editor_scene_start_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    UNUSED(instance);
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void mfc_editor_scene_start_on_exit(void* context) {
    MfcEditorApp* instance = context;
    UNUSED(instance);
    UNUSED(context);
}
