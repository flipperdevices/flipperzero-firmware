#include "../nfc_app_i.h"

void nfc_scene_mf_classic_emulate_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    const MfClassicData* data = nfc_device_get_data(nfc->nfc_device, NfcProtocolMfClassic);

    Popup* popup = nfc->popup;
    popup_set_header(popup, "Emulating", 67, 13, AlignLeft, AlignTop);
    if(!furi_string_empty(nfc->file_name)) {
        nfc_text_store_set(nfc, "%s", furi_string_get_cstr(nfc->file_name));
    } else {
        nfc_text_store_set(nfc, "Mf Classic");
    }
    popup_set_icon(popup, 0, 3, &I_NFC_dolphin_emulation_47x61);
    popup_set_text(popup, nfc->text_store, 90, 28, AlignCenter, AlignTop);

    // Setup and start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc->listener = nfc_listener_alloc(nfc->nfc, NfcProtocolMfClassic, data);
    nfc_listener_start(nfc->listener, NULL, NULL);

    nfc_blink_emulate_start(nfc);
}

bool nfc_scene_mf_classic_emulate_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    UNUSED(nfc);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void nfc_scene_mf_classic_emulate_on_exit(void* context) {
    NfcApp* nfc = context;

    nfc_listener_stop(nfc->listener);
    nfc_listener_free(nfc->listener);

    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
