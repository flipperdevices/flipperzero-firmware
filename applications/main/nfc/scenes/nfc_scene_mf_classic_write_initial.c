#include "../nfc_app_i.h"

enum {
    NfcSceneMfClassicWriteInitialStateCardSearch,
    NfcSceneMfClassicWriteInitialStateCardFound,
};

// bool nfc_mf_classic_write_initial_worker_callback(NfcWorkerEvent event, void* context) {
//     furi_assert(context);

//     NfcApp* nfc = context;
//     view_dispatcher_send_custom_event(instance->view_dispatcher, event);

//     return true;
// }

static void nfc_scene_mf_classic_write_initial_setup_view(NfcApp* instance) {
    Popup* popup = instance->popup;
    popup_reset(popup);
    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneMfClassicWriteInitial);

    if(state == NfcSceneMfClassicWriteInitialStateCardSearch) {
        popup_set_text(
            instance->popup, "Apply the initial\ncard only", 128, 32, AlignRight, AlignCenter);
        popup_set_icon(instance->popup, 0, 8, &I_NFC_manual_60x50);
    } else {
        popup_set_header(popup, "Writing\nDon't move...", 52, 32, AlignLeft, AlignCenter);
        popup_set_icon(popup, 12, 23, &A_Loading_24);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);
}

void nfc_scene_mf_classic_write_initial_on_enter(void* context) {
    NfcApp* instance = context;
    dolphin_deed(DolphinDeedNfcEmulate);

    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcSceneMfClassicWriteInitial,
        NfcSceneMfClassicWriteInitialStateCardSearch);
    nfc_scene_mf_classic_write_initial_setup_view(instance);

    // Setup and start worker
    nfc_blink_emulate_start(instance);
}

bool nfc_scene_mf_classic_write_initial_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    UNUSED(instance);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        // if(event.event == NfcWorkerEventSuccess) {
        //     scene_manager_next_scene(instance->scene_manager, NfcSceneMfClassicWriteInitialSuccess);
        //     consumed = true;
        // } else if(event.event == NfcWorkerEventFail) {
        //     scene_manager_next_scene(instance->scene_manager, NfcSceneMfClassicWriteInitialFail);
        //     consumed = true;
        // } else if(event.event == NfcWorkerEventWrongCard) {
        //     scene_manager_next_scene(instance->scene_manager, NfcSceneMfClassicWrongCard);
        //     consumed = true;
        // } else if(event.event == NfcWorkerEventCardDetected) {
        //     scene_manager_set_scene_state(
        //         instance->scene_manager,
        //         NfcSceneMfClassicWriteInitial,
        //         NfcSceneMfClassicWriteInitialStateCardFound);
        //     nfc_scene_mf_classic_write_initial_setup_view(nfc);
        //     consumed = true;
        // } else if(event.event == NfcWorkerEventNoCardDetected) {
        //     scene_manager_set_scene_state(
        //         instance->scene_manager,
        //         NfcSceneMfClassicWriteInitial,
        //         NfcSceneMfClassicWriteInitialStateCardSearch);
        //     nfc_scene_mf_classic_write_initial_setup_view(nfc);
        //     consumed = true;
        // }
    }
    return consumed;
}

void nfc_scene_mf_classic_write_initial_on_exit(void* context) {
    NfcApp* instance = context;

    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcSceneMfClassicWriteInitial,
        NfcSceneMfClassicWriteInitialStateCardSearch);
    // Clear view
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
