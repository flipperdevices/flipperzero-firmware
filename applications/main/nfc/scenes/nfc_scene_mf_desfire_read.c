#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcWorkerEventMfDesfireReadSuccess,
};

static NfcCommand nfc_scene_mf_desfire_read_worker_callback(NfcPollerEvent event, void* context) {
    furi_assert(event.protocol_type == NfcProtocolTypeMfDesfire);
    furi_assert(context);

    NfcApp* instance = context;
    MfDesfirePoller* mf_desfire_poller = event.poller;
    const MfDesfirePollerEvent* mf_desfire_event = event.data;

    NfcCommand command = NfcCommandContinue;

    if(mf_desfire_event->type == MfDesfirePollerEventTypeReadSuccess) {
        nfc_dev_set_protocol_data(
            instance->nfc_dev,
            NfcProtocolTypeMfDesfire,
            mf_desfire_poller_get_data(mf_desfire_poller));
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcWorkerEventMfDesfireReadSuccess);
        command = NfcCommandStop;
    }

    return command;
}

void nfc_scene_mf_desfire_read_on_enter(void* context) {
    NfcApp* instance = context;

    // Setup view
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    nfc_poller_manager_start(
        instance->poller_manager,
        NfcProtocolTypeMfDesfire,
        nfc_scene_mf_desfire_read_worker_callback,
        instance);

    nfc_blink_read_start(instance);
}

bool nfc_scene_mf_desfire_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventMfDesfireReadSuccess) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneMfDesfireReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_desfire_read_on_exit(void* context) {
    NfcApp* instance = context;

    nfc_poller_manager_stop(instance->poller_manager);
    // Clear view
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
