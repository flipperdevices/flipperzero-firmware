#include "../seader_i.h"
enum SubmenuIndex {
    SubmenuIndexReadPicopass,
    SubmenuIndexRead14a,
    SubmenuIndexSaved,
    SubmenuIndexFwVersion,
};

void seader_scene_sam_present_submenu_callback(void* context, uint32_t index) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, index);
}

void seader_scene_sam_present_on_enter(void* context) {
    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;

    Submenu* submenu = seader->submenu;

    submenu_add_item(
        submenu,
        "Read Picopass",
        SubmenuIndexReadPicopass,
        seader_scene_sam_present_submenu_callback,
        seader);
    submenu_add_item(
        submenu,
        "Read 14443A",
        SubmenuIndexRead14a,
        seader_scene_sam_present_submenu_callback,
        seader);
    submenu_add_item(
        submenu, "Load", SubmenuIndexSaved, seader_scene_sam_present_submenu_callback, seader);

    if(seader_worker->sam_version[0] != 0 && seader_worker->sam_version[1] != 0) {
        FuriString* fw_str = furi_string_alloc();
        furi_string_cat_printf(
            fw_str, "FW %d.%d", seader_worker->sam_version[0], seader_worker->sam_version[1]);
        submenu_add_item(
            submenu,
            furi_string_get_cstr(fw_str),
            SubmenuIndexFwVersion,
            seader_scene_sam_present_submenu_callback,
            seader);
        furi_string_free(fw_str);
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(seader->scene_manager, SeaderSceneSamPresent));

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewMenu);
}

bool seader_scene_sam_present_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexReadPicopass) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexReadPicopass);
            scene_manager_next_scene(seader->scene_manager, SeaderSceneReadPicopass);
            consumed = true;
        } else if(event.event == SubmenuIndexRead14a) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexRead14a);
            scene_manager_next_scene(seader->scene_manager, SeaderSceneRead14a);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneFileSelect);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_stop(seader->scene_manager);
        view_dispatcher_stop(seader->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void seader_scene_sam_present_on_exit(void* context) {
    Seader* seader = context;
    submenu_reset(seader->submenu);
}
