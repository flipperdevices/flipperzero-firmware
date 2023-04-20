#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcWorkerEventMfUltralightReadSuccess,
};

void nfc_scene_mf_ultralight_read_worker_callback(MfUltralightPollerEvent event, void* context) {
    NfcApp* nfc = context;

    if(event.type == MfUltralightPollerEventTypeReadSuccess) {
        mf_ultralight_poller_stop(nfc->mf_ul_poller);
        view_dispatcher_send_custom_event(
            nfc->view_dispatcher, NfcWorkerEventMfUltralightReadSuccess);
    } else if(event.type == MfUltralightPollerEventTypeAuthRequest) {
        MfUltralightData* data = &nfc->nfc_dev_data.mf_ul_data;
        mf_ultralight_poller_get_data(nfc->mf_ul_poller, data);
        if(nfc->mf_ul_auth->type == MfUltralightAuthTypeXiaomii) {
            if(mf_ultralight_generate_xiaomi_pass(
                   nfc->mf_ul_auth, data->nfca_data.uid, data->nfca_data.uid_len)) {
                event.data->auth_context.skip_auth = false;
            }
        } else if(nfc->mf_ul_auth->type == MfUltralightAuthTypeAmiibo) {
            if(mf_ultralight_generate_amiibo_pass(
                   nfc->mf_ul_auth, data->nfca_data.uid, data->nfca_data.uid_len)) {
                event.data->auth_context.skip_auth = false;
            }
        } else if(nfc->mf_ul_auth->type == MfUltralightAuthTypeManual) {
            event.data->auth_context.skip_auth = false;
        }
        if(!event.data->auth_context.skip_auth) {
            event.data->auth_context.password = nfc->mf_ul_auth->password;
        }
    } else if(event.type == MfUltralightPollerEventTypeAuthSuccess) {
        nfc->mf_ul_auth->pack = event.data->pack;
    } else {
        furi_delay_ms(100);
    }
}

void nfc_scene_mf_ultralight_read_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);

    mf_ultralight_poller_read(
        nfc->mf_ul_poller, nfc_scene_mf_ultralight_read_worker_callback, nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_mf_ultralight_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventMfUltralightReadSuccess) {
            notification_message(nfc->notifications, &sequence_success);
            mf_ultralight_poller_get_data(nfc->mf_ul_poller, &nfc->nfc_dev_data.mf_ul_data);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_ultralight_read_on_exit(void* context) {
    NfcApp* nfc = context;

    mf_ultralight_poller_reset(nfc->mf_ul_poller);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
