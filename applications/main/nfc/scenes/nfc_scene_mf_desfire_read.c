#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcWorkerEventMfDesfireReadSuccess,
};

MfDesfirePollerCommand
    nfc_scene_mf_desfire_read_worker_callback(MfDesfirePollerEvent event, void* context) {
    UNUSED(event);
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(
        nfc->view_dispatcher, NfcWorkerEventMfDesfireReadSuccess);

    MfDesfirePollerCommand command = MfDesfirePollerCommandContinue;
    return command;
}

void nfc_scene_mf_desfire_read_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);

    mf_desfire_poller_read(
        nfc->mf_desfire_poller, nfc_scene_mf_desfire_read_worker_callback, nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_mf_desfire_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventMfDesfireReadSuccess) {
            notification_message(nfc->notifications, &sequence_success);
            mf_desfire_poller_get_data(nfc->mf_desfire_poller, &nfc->nfc_dev_data.mf_desfire_data);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_desfire_read_on_exit(void* context) {
    NfcApp* nfc = context;

    mf_desfire_poller_stop(nfc->mf_desfire_poller);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
