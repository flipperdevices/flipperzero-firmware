#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexEditUid,
    SubmenuIndexDetectReader,
    SubmenuIndexWrite,
    SubmenuIndexUpdate,
    SubmenuIndexRename,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
    SubmenuIndexRestoreOriginal,
    SubmenuIndexMfUlUnlockByReader,
    SubmenuIndexMfUlUnlockByPassword,
};

void nfc_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_saved_menu_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    const NfcProtocol protocol = nfc_device_get_protocol(nfc->nfc_device);

    if(protocol == NfcProtocolIso14443_3a) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexEmulate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu, "Edit UID", SubmenuIndexEditUid, nfc_scene_saved_menu_submenu_callback, nfc);

    } else if(protocol == NfcProtocolMfDesfire) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexEmulate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
    } else if((protocol == NfcProtocolMfUltralight) || (protocol == NfcProtocolMfClassic)) {
        submenu_add_item(
            submenu, "Emulate", SubmenuIndexEmulate, nfc_scene_saved_menu_submenu_callback, nfc);
    }

    if(protocol == NfcProtocolMfClassic) {
        // TODO
        // if(!mifare_classic_is_card_read(&nfc->dev->dev_data.mf_classic_data))
        submenu_add_item(
            submenu,
            "Detect Reader",
            SubmenuIndexDetectReader,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu,
            "Write to Initial Card",
            SubmenuIndexWrite,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu,
            "Update from Initial Card",
            SubmenuIndexUpdate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
    }

    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, nfc_scene_saved_menu_submenu_callback, nfc);

    if(protocol == NfcProtocolMfUltralight) {
        const MfUltralightData* mfu_data =
            nfc_device_get_data(nfc->nfc_device, NfcProtocolMfUltralight);
        if(!mf_ultralight_is_all_data_read(mfu_data)) {
            submenu_add_item(
                submenu,
                "Unlock with Reader",
                SubmenuIndexMfUlUnlockByReader,
                nfc_scene_saved_menu_submenu_callback,
                nfc);
            submenu_add_item(
                submenu,
                "Unlock with Password",
                SubmenuIndexMfUlUnlockByPassword,
                nfc_scene_saved_menu_submenu_callback,
                nfc);
        }
    }
    // TODO
    // if(nfc_is_shadow_file_exist)
    submenu_add_item(
        submenu,
        "Restore to original",
        SubmenuIndexRestoreOriginal,
        nfc_scene_saved_menu_submenu_callback,
        nfc);

    submenu_add_item(
        submenu, "Rename", SubmenuIndexRename, nfc_scene_saved_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Delete", SubmenuIndexDelete, nfc_scene_saved_menu_submenu_callback, nfc);
    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneSavedMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    const NfcProtocol protocol = nfc_device_get_protocol(nfc->nfc_device);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneSavedMenu, event.event);
        if(event.event == SubmenuIndexEmulate) {
            if(protocol == NfcProtocolMfUltralight) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightEmulate);
            } else if(protocol == NfcProtocolMfClassic) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaEmulate);
            }
            dolphin_deed(DolphinDeedNfcEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexDetectReader) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            dolphin_deed(DolphinDeedNfcDetectReader);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexUpdate) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexRename) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexEditUid) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSetUid);
            consumed = true;
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneDelete);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneInfo);
            consumed = true;
        } else if(event.event == SubmenuIndexRestoreOriginal) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUlUnlockByReader) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUlUnlockByPassword) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightUnlockMenu);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_saved_menu_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
