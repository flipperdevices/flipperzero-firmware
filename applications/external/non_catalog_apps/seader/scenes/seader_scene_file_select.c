#include "../seader_i.h"
#include "../seader_credential.h"

void seader_scene_file_select_on_enter(void* context) {
    Seader* seader = context;
    // Process file_select return
    seader_credential_set_loading_callback(seader->credential, seader_show_loading_popup, seader);
    if(seader_file_select(seader->credential)) {
        scene_manager_next_scene(seader->scene_manager, SeaderSceneSavedMenu);
    } else {
        scene_manager_search_and_switch_to_previous_scene(seader->scene_manager, SeaderSceneStart);
    }
    seader_credential_set_loading_callback(seader->credential, NULL, seader);
}

bool seader_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void seader_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
