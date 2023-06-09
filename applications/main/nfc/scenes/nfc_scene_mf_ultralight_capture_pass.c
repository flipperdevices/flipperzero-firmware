#include "../nfc_app_i.h"

MfUltralightListenerCommand nfc_scene_mf_ultralight_capture_pass_worker_callback(
    MfUltralightListenerEvent event,
    void* context) {
    NfcApp* nfc = context;

    if(event.type == MfUltralightListenerEventTypeAuth) {
        nfc->mf_ul_auth->password = event.data->password;
        view_dispatcher_send_custom_event(nfc->view_dispatcher, MfUltralightListenerEventTypeAuth);
    }

    return MfUltralightListenerCommandContinue;
}

void nfc_scene_mf_ultralight_capture_pass_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    widget_add_string_multiline_element(
        nfc->widget,
        54,
        30,
        AlignLeft,
        AlignCenter,
        FontPrimary,
        "Touch the\nreader to get\npassword...");
    widget_add_icon_element(nfc->widget, 0, 15, &I_Modern_reader_18x34);
    widget_add_icon_element(nfc->widget, 20, 12, &I_Move_flipper_26x39);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);

    // Start worker
    mf_ultralight_listener_start(
        nfc->mf_ul_listener,
        nfc->nfc_dev_data.mf_ul_data,
        nfc_scene_mf_ultralight_capture_pass_worker_callback,
        nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_mf_ultralight_capture_pass_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if((event.event == MfUltralightListenerEventTypeAuth)) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightUnlockWarn);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_ultralight_capture_pass_on_exit(void* context) {
    NfcApp* nfc = context;

    // Stop worker
    mf_ultralight_listener_stop(nfc->mf_ul_listener);
    // Clear view
    widget_reset(nfc->widget);

    nfc_blink_stop(nfc);
}
