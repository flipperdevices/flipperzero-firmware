#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

bool nfc_crocodile_scene_transfer_send_callback(void* context) {
    furi_assert(context);
    NfcCrocodile* nfc_crocodile = context;
    furi_assert(nfc_crocodile->view_dispatcher);
    view_dispatcher_send_custom_event(nfc_crocodile->view_dispatcher, 0);
    return true;
}

void nfc_crocodile_scene_transfer_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    Popup* popup = nfc_crocodile->popup;
    popup_reset(popup);
    popup_set_icon(nfc_crocodile->popup, 0, 8, &I_NFC_dolphin_emulation_47x61);
    popup_set_text(
        nfc_crocodile->popup, "Apply phone to\nthe back", 128, 32, AlignRight, AlignCenter);

    nfc_crocodile_worker_start(
        nfc_crocodile->nfc_crocodile_worker,
        NfcCrocodileWorkerStateReady,
        nfc_crocodile->storage_type,
        nfc_crocodile->card_content,
        nfc_crocodile_scene_transfer_send_callback,
        context);

    view_dispatcher_switch_to_view(nfc_crocodile->view_dispatcher, NfcCrocodileViewPopup);
}

bool nfc_crocodile_scene_transfer_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == 0) {
            scene_manager_next_scene(nfc_crocodile->scene_manager, NfcCrocodileSceneTransferDone);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_crocodile_scene_transfer_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    nfc_crocodile_worker_stop(nfc_crocodile->nfc_crocodile_worker);
    popup_reset(nfc_crocodile->popup);
}