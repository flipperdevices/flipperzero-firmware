#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexEmulate,
    SubmenuIndexDetectReader,
    SubmenuIndexInfo,
};

void nfc_scene_mf_classic_menu_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_mf_classic_menu_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, nfc_scene_mf_classic_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Emulate", SubmenuIndexEmulate, nfc_scene_mf_classic_menu_submenu_callback, nfc);

    const MfClassicData* mfc_data =
        nfc_dev_get_protocol_data(nfc->nfc_dev, NfcProtocolTypeMfClassic);
    if(!mf_classic_is_card_read(mfc_data)) {
        submenu_add_item(
            submenu,
            "Detect Reader",
            SubmenuIndexDetectReader,
            nfc_scene_mf_classic_menu_submenu_callback,
            nfc);
    }
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, nfc_scene_mf_classic_menu_submenu_callback, nfc);

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfClassicMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_mf_classic_menu_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneMfClassicMenu, event.event);
        if(event.event == SubmenuIndexSave) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexEmulate) {
            if(scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneSetType)) {
                dolphin_deed(DolphinDeedNfcAddEmulate);
            } else {
                dolphin_deed(DolphinDeedNfcEmulate);
            }
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexDetectReader) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            dolphin_deed(DolphinDeedNfcDetectReader);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneInfo);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_mf_classic_menu_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    submenu_reset(nfc->submenu);
}
