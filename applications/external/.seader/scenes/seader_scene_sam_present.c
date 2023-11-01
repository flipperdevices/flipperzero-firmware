#include "../seader_i.h"
enum SubmenuIndex {
    SubmenuIndexReadPicopass,
    SubmenuIndexRead14a,
    SubmenuIndexSaved,
    SubmenuIndexSamInfo,
    SubmenuIndexFwVersion,
};

static uint8_t fwChecks = 3;

void seader_scene_sam_present_submenu_callback(void* context, uint32_t index) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, index);
}

void seader_scene_sam_present_on_update(void* context) {
    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;

    Submenu* submenu = seader->submenu;
    submenu_reset(submenu);

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
        fwChecks = 0;
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(seader->scene_manager, SeaderSceneSamPresent));

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewMenu);
}

void seader_scene_sam_present_on_enter(void* context) {
    seader_scene_sam_present_on_update(context);
}

bool seader_scene_sam_present_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexReadPicopass) {
            if(seader->is_debug_enabled) {
                seader->credential->type = SeaderCredentialTypePicopass;
                scene_manager_set_scene_state(
                    seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexReadPicopass);
                scene_manager_next_scene(seader->scene_manager, SeaderSceneUart);
            } else {
                scene_manager_set_scene_state(
                    seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexReadPicopass);
                scene_manager_next_scene(seader->scene_manager, SeaderSceneReadPicopass);
            }
            consumed = true;
        } else if(event.event == SubmenuIndexRead14a) {
            if(seader->is_debug_enabled) {
                seader->credential->type = SeaderCredentialType14A;
                scene_manager_set_scene_state(
                    seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexRead14a);
                scene_manager_next_scene(seader->scene_manager, SeaderSceneUart);
            } else {
                scene_manager_set_scene_state(
                    seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexRead14a);
                scene_manager_next_scene(seader->scene_manager, SeaderSceneRead14a);
            }
            consumed = true;
        } else if(event.event == SubmenuIndexSamInfo) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneSamPresent, SubmenuIndexSamInfo);
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSamInfo);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneFileSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexFwVersion) {
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_stop(seader->scene_manager);
        view_dispatcher_stop(seader->view_dispatcher);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        SeaderWorker* seader_worker = seader->worker;
        if(fwChecks > 0 && seader_worker->sam_version[0] != 0 &&
           seader_worker->sam_version[1] != 0) {
            fwChecks--;
            seader_scene_sam_present_on_update(context);
        }
    }

    return consumed;
}

void seader_scene_sam_present_on_exit(void* context) {
    Seader* seader = context;
    submenu_reset(seader->submenu);
}
