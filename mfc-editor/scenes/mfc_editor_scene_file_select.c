#include "../mfc_editor_app_i.h"

void mfc_editor_scene_file_select_on_enter(void* context) {
    MfcEditorApp* instance = context;

    if(mfc_editor_prompt_load_file(instance)) {
        if(nfc_device_get_protocol(instance->nfc_device) == NfcProtocolMfClassic) {
            scene_manager_set_scene_state(instance->scene_manager, MfcEditorSceneSectorSelect, 0);
            scene_manager_next_scene(instance->scene_manager, MfcEditorSceneSectorSelect);
        } else {
            scene_manager_next_scene(instance->scene_manager, MfcEditorSceneInvalidFile);
        }
    } else {
        scene_manager_previous_scene(instance->scene_manager);
    }
}

bool mfc_editor_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void mfc_editor_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
