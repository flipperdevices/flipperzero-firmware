#include "../nfc_i.h"

void nfc_scene_mf_classic_keys_warn_duplicate_popup_callback(void* context) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventViewExit);
}

void nfc_scene_mf_classic_keys_warn_duplicate_on_enter(void* context) {
    Nfc* nfc = context;

    // Setup view
    Popup* popup = nfc->popup;
    popup_set_icon(popup, 67, 12, &I_DolphinFirstStart7_61x51);
    popup_set_text(
        popup,
        "Key already exists\nin user dictionary.\n(Duplicate entry)",
        38,
        35,
        AlignCenter,
        AlignBottom);
    popup_set_timeout(popup, 5000);
    popup_set_context(popup, nfc);
    popup_set_callback(popup, nfc_scene_mf_classic_keys_warn_duplicate_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
}

bool nfc_scene_mf_classic_keys_warn_duplicate_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventViewExit) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                nfc->scene_manager,
                scene_manager_get_scene_state(
                    nfc->scene_manager, NfcSceneMfClassicKeysWarnDuplicate));
        }
    }
    return consumed;
}

void nfc_scene_mf_classic_keys_warn_duplicate_on_exit(void* context) {
    Nfc* nfc = context;

    popup_reset(nfc->popup);
}
