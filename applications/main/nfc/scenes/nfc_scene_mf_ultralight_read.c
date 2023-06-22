#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcWorkerEventMfUltralightReadSuccess,
};

NfcCommand nfc_scene_mf_ultralight_read_worker_callback(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.poller);
    furi_assert(event.data);

    NfcApp* instance = context;
    MfUltralightPollerEvent* mfu_event = event.data;
    MfUltralightPoller* mfu_poller = event.poller;

    NfcCommand command = NfcCommandContinue;

    if(mfu_event->type == MfUltralightPollerEventTypeReadSuccess) {
        nfc_dev_set_protocol_data(
            instance->nfc_dev, event.protocol_type, mf_ultralight_poller_get_data(mfu_poller));
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcWorkerEventMfUltralightReadSuccess);
        command = NfcCommandStop;
    } else if(mfu_event->type == MfUltralightPollerEventTypeAuthRequest) {
        nfc_dev_set_protocol_data(
            instance->nfc_dev, event.protocol_type, mf_ultralight_poller_get_data(mfu_poller));
        const MfUltralightData* data =
            nfc_dev_get_protocol_data(instance->nfc_dev, NfcProtocolTypeMfUltralight);
        if(instance->mf_ul_auth->type == MfUltralightAuthTypeXiaomii) {
            if(mf_ultralight_generate_xiaomi_pass(
                   instance->mf_ul_auth, data->nfca_data->uid, data->nfca_data->uid_len)) {
                mfu_event->data->auth_context.skip_auth = false;
            }
        } else if(instance->mf_ul_auth->type == MfUltralightAuthTypeAmiibo) {
            if(mf_ultralight_generate_amiibo_pass(
                   instance->mf_ul_auth, data->nfca_data->uid, data->nfca_data->uid_len)) {
                mfu_event->data->auth_context.skip_auth = false;
            }
        } else if(instance->mf_ul_auth->type == MfUltralightAuthTypeManual) {
            mfu_event->data->auth_context.skip_auth = false;
        } else {
            mfu_event->data->auth_context.skip_auth = true;
        }
        if(!mfu_event->data->auth_context.skip_auth) {
            mfu_event->data->auth_context.password = instance->mf_ul_auth->password;
        }
    } else if(mfu_event->type == MfUltralightPollerEventTypeAuthSuccess) {
        instance->mf_ul_auth->pack = mfu_event->data->auth_context.pack;
    }

    return command;
}

void nfc_scene_mf_ultralight_read_on_enter(void* context) {
    NfcApp* instance = context;

    // Setup view
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    nfc_poller_manager_start(
        instance->poller_manager,
        NfcProtocolTypeMfUltralight,
        nfc_scene_mf_ultralight_read_worker_callback,
        instance);

    nfc_blink_read_start(instance);
}

bool nfc_scene_mf_ultralight_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventMfUltralightReadSuccess) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneMfUltralightReadSuccess);
            dolphin_deed(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_ultralight_read_on_exit(void* context) {
    NfcApp* instance = context;

    nfc_poller_manager_stop(instance->poller_manager);
    // Clear view
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
