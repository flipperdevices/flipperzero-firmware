#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexEmulateUid,
    SubmenuIndexInfo,
};

void nfc_scene_mf_desfire_menu_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_mf_desfire_menu_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, nfc_scene_mf_desfire_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu,
        "Emulate UID",
        SubmenuIndexEmulateUid,
        nfc_scene_mf_desfire_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, nfc_scene_mf_desfire_menu_submenu_callback, nfc);

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfDesfireMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_mf_desfire_menu_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSave) {
            // TODO: Implement saving
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexEmulateUid) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaEmulate);
            if(scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneNotImplemented)) {
                DOLPHIN_DEED(DolphinDeedNfcAddEmulate);
            } else {
                DOLPHIN_DEED(DolphinDeedNfcEmulate);
            }
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            // TODO: Implement info
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_mf_desfire_menu_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    submenu_reset(nfc->submenu);
}
