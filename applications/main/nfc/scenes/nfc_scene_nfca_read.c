#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_nfca_read_worker_callback(NfcaPollerEvent event, void* context) {
    NfcApp* nfc = context;

    if(event.type == NfcaPollerEventTypeActivated) {
        nfca_poller_stop(nfc->nfca_poller);
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcWorkerEventReadUidNfcA);
    }
}

void nfc_scene_nfca_read_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    nfc_scene_nfca_read_set_state(nfc, NfcSceneReadStateDetecting);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    
    nfc->nfca_poller = nfca_poller_alloc();
    nfca_poller_start(nfc->nfca_poller, nfc_scene_nfca_read_worker_callback, nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_nfca_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventReadUidNfcA) {
            notification_message(nfc->notifications, &sequence_success);
            nfca_poller_get_data(nfc->nfca_poller, &nfc->nfca_data);
            nfca_poller_free(nfc->nfca_poller);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_nfca_read_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    popup_reset(nfc->popup);
    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneRead, NfcSceneReadStateIdle);

    nfc_blink_stop(nfc);
}
