#include "../nfc_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcSceneMfClassicWriteStateCardSearch,
    NfcSceneMfClassicWriteStateCardFound,
};

bool nfc_mf_classic_write_worker_callback(NfcWorkerEvent event, void* context) {
    furi_assert(context);

    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);

    return true;
}

static void nfc_scene_mf_classic_write_setup_view(Nfc* nfc) {
    Popup* popup = nfc->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfClassicWrite);

    if(state == NfcSceneMfClassicWriteStateCardSearch) {
        popup_set_header(popup, "Place card to write", 64, 32, AlignCenter, AlignCenter);
    } else {
        if(strcmp(nfc->dev->dev_name, "")) {
            nfc_text_store_set(nfc, "Writing\n%s", nfc->dev->dev_name);
        } else {
            nfc_text_store_set(nfc, "Writing\nMf Classic", nfc->dev->dev_name);
        }
        popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
        popup_set_header(popup, nfc->text_store, 56, 31, AlignLeft, AlignTop);
    }

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
}

void nfc_scene_mf_classic_write_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcEmulate);

    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneMfClassicWrite, NfcSceneMfClassicWriteStateCardSearch);
    nfc_scene_mf_classic_write_setup_view(nfc);

    // Setup and start worker
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

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventWrongCard) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicWrongCard);
            consumed = true;
        } else if(event.event == NfcWorkerEventCardDetected) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneMfClassicWrite, NfcSceneMfClassicWriteStateCardFound);
            nfc_scene_mf_classic_write_setup_view(nfc);
            consumed = true;
        } else if(event.event == NfcWorkerEventNoCardDetected) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneMfClassicWrite, NfcSceneMfClassicWriteStateCardSearch);
            nfc_scene_mf_classic_write_setup_view(nfc);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_classic_write_on_exit(void* context) {
    Nfc* nfc = context;

    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneMfClassicWrite, NfcSceneMfClassicWriteStateCardSearch);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
