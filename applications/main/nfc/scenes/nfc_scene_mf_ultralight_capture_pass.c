#include "../nfc_app_i.h"

NfcCommand
    nfc_scene_mf_ultralight_capture_pass_worker_callback(NfcGenericEvent event, void* context) {
    NfcApp* nfcApp = context;
    MfUltralightListenerEvent* mfu_event = event.data;
    MfUltralightAuth* mauth = nfcApp->mf_ul_auth;

    if(mfu_event->type == MfUltralightListenerEventTypeAuth) {
        mauth->password = mfu_event->data->password;
        view_dispatcher_send_custom_event(
            nfcApp->view_dispatcher, MfUltralightListenerEventTypeAuth);
    }

    return NfcCommandContinue;
}

void nfc_scene_mf_ultralight_capture_pass_on_enter(void* context) {
    NfcApp* nfcApp = context;

    // Setup view
    widget_add_string_multiline_element(
        nfcApp->widget,
        54,
        30,
        AlignLeft,
        AlignCenter,
        FontPrimary,
        "Touch the\nreader to get\npassword...");
    widget_add_icon_element(nfcApp->widget, 0, 15, &I_Modern_reader_18x34);
    widget_add_icon_element(nfcApp->widget, 20, 12, &I_Move_flipper_26x39);
    view_dispatcher_switch_to_view(nfcApp->view_dispatcher, NfcViewWidget);

    // Start worker
    const MfUltralightData* data =
        nfc_device_get_data(nfcApp->nfc_device, NfcProtocolMfUltralight);
    nfcApp->listener = nfc_listener_alloc(nfcApp->nfc, NfcProtocolMfUltralight, data);
    nfc_listener_start(
        nfcApp->listener, nfc_scene_mf_ultralight_capture_pass_worker_callback, nfcApp);

    nfc_blink_read_start(nfcApp);
}

bool nfc_scene_mf_ultralight_capture_pass_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MfUltralightListenerEventTypeAuth) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightUnlockWarn);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_ultralight_capture_pass_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    nfc_listener_stop(nfc->listener);
    nfc_listener_free(nfc->listener);
    widget_reset(nfc->widget);

    nfc_blink_stop(nfc);
}
