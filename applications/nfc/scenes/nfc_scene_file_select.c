#include <nfc/scenes/nfc_scene_file_select.h>

#include <furi.h>

#include "../nfc_i.h"

const void nfc_scene_file_select_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;
    // Process file_select return
    if(nfc_file_select(&nfc->device)) {
        scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_saved_menu);
        scene_manager_next_scene(nfc->scene_manager);
    } else {
        scene_manager_search_previous_scene(nfc->scene_manager, NfcSceneStart);
    }
}

const bool nfc_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    return false;
}

const void nfc_scene_file_select_on_exit(void* context) {
}

AppScene* nfc_scene_file_select_alloc() {
    AppScene* scene = furi_alloc(sizeof(AppScene));
    scene->id = NfcSceneFileSelect;
    scene->on_enter = nfc_scene_file_select_on_enter;
    scene->on_event = nfc_scene_file_select_on_event;
    scene->on_exit = nfc_scene_file_select_on_exit;

    return scene;
}

void nfc_scene_file_select_free(AppScene* scene) {
    free(scene);
}
