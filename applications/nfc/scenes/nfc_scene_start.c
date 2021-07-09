#include "nfc_scene_start.h"
#include "../nfc_i.h"

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>

enum SubmenuIndex {
    SubmenuIndexRead,
    SubmenuIndexRunScript,
    SubmenuIndexSaved,
    SubmenuIndexAddManualy,
    SubmenuIndexDebug,
};

void nfc_scene_start_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->nfc_common.view_dispatcher, index);
}

const void nfc_scene_start_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu, "Read card", SubmenuIndexRead, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(
        submenu,
        "Run special action",
        SubmenuIndexRunScript,
        nfc_scene_start_submenu_callback,
        nfc);
    submenu_add_item(
        submenu, "Saved cards", SubmenuIndexSaved, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Add manually", SubmenuIndexAddManualy, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(submenu, "Debug", SubmenuIndexDebug, nfc_scene_start_submenu_callback, nfc);
    submenu_set_selected_item(nfc->submenu, nfc->scene_start->state);

    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);
}

const bool nfc_scene_start_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            nfc->scene_start->state = SubmenuIndexRead;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_read_card);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexRunScript) {
            nfc->scene_start->state = SubmenuIndexRunScript;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_scripts_menu);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexSaved) {
            nfc->scene_start->state = SubmenuIndexSaved;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_file_select);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexAddManualy) {
            nfc->scene_start->state = SubmenuIndexAddManualy;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_set_type);
            return scene_manager_next_scene(nfc->scene_manager);
        } else if(event.event == SubmenuIndexDebug) {
            nfc->scene_start->state = SubmenuIndexDebug;
            scene_manager_add_next_scene(nfc->scene_manager, nfc->scene_debug_menu);
            return scene_manager_next_scene(nfc->scene_manager);
        }
    }
    return false;
}

const void nfc_scene_start_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    submenu_clean(nfc->submenu);
}

AppScene* nfc_scene_start_alloc() {
    AppScene* scene = furi_alloc(sizeof(AppScene));
    scene->id = NfcSceneStart;
    scene->on_enter = nfc_scene_start_on_enter;
    scene->on_event = nfc_scene_start_on_event;
    scene->on_exit = nfc_scene_start_on_exit;

    return scene;
}

void nfc_scene_start_free(AppScene* scene) {
    free(scene);
}