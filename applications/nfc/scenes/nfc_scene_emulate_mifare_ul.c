#include "../nfc_i.h"

#define NFC_EMULATE_MF_UL_CUSTOM_CALLBACK (0UL)

void nfc_emulate_mifare_ul_worker_callback(void* context) {
    Nfc* nfc = (Nfc*)context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NFC_EMULATE_MF_UL_CUSTOM_CALLBACK);
}

const void nfc_scene_emulate_mifare_ul_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;

    // Setup view
    Popup* popup = nfc->popup;
    if(strcmp(nfc->dev.dev_name, "")) {
        nfc_text_store_set(nfc, "%s", nfc->dev.dev_name);
    }
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    popup_set_header(popup, "Emulating\nMf Ultralight", 56, 31, AlignLeft, AlignTop);

    // Setup and start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateEmulateMifareUl,
        &nfc->dev.dev_data,
        nfc_emulate_mifare_ul_worker_callback,
        nfc);
}

const bool nfc_scene_emulate_mifare_ul_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        notification_message(nfc->notifications, &sequence_blink_blue_10);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(nfc->dev.dev_data.mf_ul_data.data_changed) {
            // TODO move to save select scene
        }
    }
    return consumed;
}

const void nfc_scene_emulate_mifare_ul_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    // Stop worker
    nfc_worker_stop(nfc->worker);

    // Clear view
    Popup* popup = nfc->popup;
    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignBottom);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 0, NULL);
}
