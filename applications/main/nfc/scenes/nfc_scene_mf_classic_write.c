#include "../nfc_i.h"
#include <dolphin/dolphin.h>

bool nfc_mf_classic_write_worker_callback(NfcWorkerEvent event, void* context) {
    furi_assert(context);

    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);

    return true;
}

void nfc_scene_mf_classic_write_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcEmulate);

    // Setup view
    Popup* popup = nfc->popup;
    if(strcmp(nfc->dev->dev_name, "")) {
        nfc_text_store_set(nfc, "Writing\n%s", nfc->dev->dev_name);
    } else {
        nfc_text_store_set(nfc, "Writing\nMf Classic", nfc->dev->dev_name);
    }
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    popup_set_header(popup, nfc->text_store, 56, 31, AlignLeft, AlignTop);

    // Setup and start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateMfClassicWrite,
        &nfc->dev->dev_data,
        nfc_mf_classic_write_worker_callback,
        nfc);
    nfc_blink_emulate_start(nfc);
}

bool nfc_scene_mf_classic_write_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        // Stop worker
        nfc_worker_stop(nfc->worker);
        consumed = false;
    }
    return consumed;
}

void nfc_scene_mf_classic_write_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
