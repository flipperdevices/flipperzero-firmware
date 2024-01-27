#include "../mfc_editor_app_i.h"

void mfc_editor_scene_file_select_on_enter(void* context) {
    MfcEditorApp* instance = context;

    // File select scene should repeat itself if the file load failed
    // or if the user quit the shadow file prompt, not the file selector
    MfcEditorPromptResponse prompt_response = MfcEditorPromptResponseFailure;
    while(prompt_response == MfcEditorPromptResponseFailure ||
          prompt_response == MfcEditorPromptResponseExitedShadow) {
        prompt_response = mfc_editor_prompt_load_file(instance);
    }

    if(prompt_response == MfcEditorPromptResponseSuccess) {
        scene_manager_set_scene_state(instance->scene_manager, MfcEditorSceneSectorSelect, 0);
        scene_manager_next_scene(instance->scene_manager, MfcEditorSceneSectorSelect);
    } else if(prompt_response == MfcEditorPromptResponseNotMfClassic) {
        scene_manager_next_scene(instance->scene_manager, MfcEditorSceneInvalidFile);
    } else {
        // Exiting from file list stops the application
        view_dispatcher_stop(instance->view_dispatcher);
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
