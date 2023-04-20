#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_mf_ultralight_unlock_warn_dialog_callback(DialogExResult result, void* context) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_mf_ultralight_unlock_warn_on_enter(void* context) {
    NfcApp* nfc = context;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_mf_ultralight_unlock_warn_dialog_callback);

    dialog_ex_set_header(dialog_ex, "Risky function!", 64, 4, AlignCenter, AlignTop);
    dialog_ex_set_text(
        dialog_ex, "Wrong password\ncan block your\ncard.", 4, 18, AlignLeft, AlignTop);
    dialog_ex_set_icon(dialog_ex, 73, 20, &I_DolphinCommon_56x48);
    dialog_ex_set_center_button_text(dialog_ex, "OK");

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

bool nfc_scene_mf_ultralight_unlock_warn_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultCenter) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            DOLPHIN_DEED(DolphinDeedNfcRead);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_mf_ultralight_unlock_warn_on_exit(void* context) {
    NfcApp* nfc = context;

    dialog_ex_reset(nfc->dialog_ex);
    nfc_text_store_clear(nfc);

    notification_message_block(nfc->notifications, &sequence_reset_green);
}
