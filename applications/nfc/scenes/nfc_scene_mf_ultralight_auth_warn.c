#include "../nfc_i.h"

void nfc_scene_mf_ultralight_auth_warn_dialog_callback(DialogExResult result, void* context) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_mf_ultralight_auth_warn_on_enter(void* context) {
    Nfc* nfc = context;
    DialogEx* dialog_ex = nfc->dialog_ex;

    MfUltralightData data = nfc->dev->dev_data.mf_ul_data;
    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_mf_ultralight_auth_warn_dialog_callback);

    if(!data.has_auth) {
        dialog_ex_set_text(
            dialog_ex, "There's no authentication on the tag", 64, 32, AlignCenter, AlignCenter);
    } else {
        dialog_ex_set_left_button_text(dialog_ex, "Cancel");
        dialog_ex_set_right_button_text(dialog_ex, "Continue");

        dialog_ex_set_header(dialog_ex, "Confirm auth", 64, 10, AlignCenter, AlignCenter);
        if(!data.auth_readable) {
            dialog_ex_set_text(
                dialog_ex,
                "Can't read auth info!\r\nTag may lock out permanently!",
                64,
                32,
                AlignCenter,
                AlignCenter);
        } else {
            char text[88];
            sprintf(
                text,
                "Auth attempts are limited!\r\nTag may lock out permanently!\r\n"
                "Limit: %d",
                data.curr_authlim);
            dialog_ex_set_text(dialog_ex, text, 64, 32, AlignCenter, AlignCenter);
        }
    }

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

bool nfc_scene_mf_ultralight_auth_warn_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == DialogExResultRight) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightAuth);
        consumed = true;
    } else if(
        event.type == SceneManagerEventTypeBack ||
        (event.type == SceneManagerEventTypeCustom && event.event == DialogExResultLeft)) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_mf_ultralight_auth_warn_on_exit(void* context) {
    Nfc* nfc = context;

    dialog_ex_reset(nfc->dialog_ex);
    submenu_reset(nfc->submenu);
}
