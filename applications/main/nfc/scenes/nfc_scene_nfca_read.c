#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcWorkerEventReadUidNfcA = 100,
};

// NfcaPollerCommand nfc_scene_nfca_read_worker_callback(NfcaPollerEvent event, void* context) {
//     NfcApp* nfc = context;

//     NfcaPollerCommand command = NfcaPollerCommandContinue;

//     if(event.type == NfcaPollerEventTypeReady) {
//         nfc_dev_set_protocol_data(
//             nfc->nfc_dev, NfcProtocolTypeIso14443_3a, nfca_poller_get_data(nfc->nfca_poller));
//         view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcWorkerEventReadUidNfcA);
//         command = NfcaPollerCommandStop;
//     }

//     return command;
// }

NfcCommand nfc_scene_nfca_read_worker_callback(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.poller);

    NfcApp* instance = context;
    NfcCommand command = NfcCommandContinue;
    NfcaPollerEvent* nfca_event = event.data;
    const NfcPollerBase* nfca_poller_api = nfc_pollers_api[NfcProtocolTypeIso14443_3a];

    if(nfca_event->type == NfcaPollerEventTypeReady) {
        nfc_dev_set_protocol_data(
            instance->nfc_dev,
            NfcProtocolTypeIso14443_3a,
            nfca_poller_api->get_data(event.poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcWorkerEventReadUidNfcA);
        command = NfcCommandStop;
    }

    return command;
}

void nfc_scene_nfca_read_on_enter(void* context) {
    NfcApp* instance = context;

    // Setup view
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    // nfca_poller_start(nfc->nfca_poller, nfc_scene_nfca_read_worker_callback, nfc);
    nfc_poller_manager_start(
        instance->poller_manager,
        NfcProtocolTypeIso14443_3a,
        nfc_scene_nfca_read_worker_callback,
        instance);

    nfc_blink_read_start(instance);
}

bool nfc_scene_nfca_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventReadUidNfcA) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_nfca_read_on_exit(void* context) {
    NfcApp* instance = context;

    // nfca_poller_stop(nfc->nfca_poller);
    nfc_poller_manager_stop(instance->poller_manager);
    // Clear view
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
