#include "../seader_i.h"

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexSavePicopass,
    SubmenuIndexSaveRFID,
    SubmenuIndexSaveSR,
    SubmenuIndexSaveMFC,
};

void seader_scene_card_menu_submenu_callback(void* context, uint32_t index) {
    Seader* seader = context;

    view_dispatcher_send_custom_event(seader->view_dispatcher, index);
}

void seader_scene_card_menu_on_enter(void* context) {
    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;
    SeaderCredential* credential = seader_worker->credential;
    Submenu* submenu = seader->submenu;

    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, seader_scene_card_menu_submenu_callback, seader);
    submenu_add_item(
        submenu,
        "Save Picopass",
        SubmenuIndexSavePicopass,
        seader_scene_card_menu_submenu_callback,
        seader);
    submenu_add_item(
        submenu,
        "Save RFID",
        SubmenuIndexSaveRFID,
        seader_scene_card_menu_submenu_callback,
        seader);
    if(credential->sio[0] == 0x30) {
        submenu_add_item(
            submenu,
            "Save SR",
            SubmenuIndexSaveSR,
            seader_scene_card_menu_submenu_callback,
            seader);
    }
    submenu_add_item(
        submenu, "Save MFC", SubmenuIndexSaveMFC, seader_scene_card_menu_submenu_callback, seader);

    submenu_set_selected_item(
        seader->submenu,
        scene_manager_get_scene_state(seader->scene_manager, SeaderSceneCardMenu));

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewMenu);
}

bool seader_scene_card_menu_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSave) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneCardMenu, SubmenuIndexSave);
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexSavePicopass) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneCardMenu, SubmenuIndexSavePicopass);
            seader->credential->save_format = SeaderCredentialSaveFormatPicopass;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexSaveRFID) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneCardMenu, SubmenuIndexSaveRFID);
            seader->credential->save_format = SeaderCredentialSaveFormatRFID;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexSaveSR) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneCardMenu, SubmenuIndexSaveSR);
            seader->credential->save_format = SeaderCredentialSaveFormatSR;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexSaveMFC) {
            scene_manager_set_scene_state(
                seader->scene_manager, SeaderSceneCardMenu, SubmenuIndexSaveMFC);
            seader->credential->save_format = SeaderCredentialSaveFormatMFC;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveName);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            seader->scene_manager, SeaderSceneSamPresent);
    }

    return consumed;
}

void seader_scene_card_menu_on_exit(void* context) {
    Seader* seader = context;

    submenu_reset(seader->submenu);
}
