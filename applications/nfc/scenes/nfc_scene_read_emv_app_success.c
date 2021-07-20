#include "../nfc_i.h"

#define NFC_SCENE_READ_SUCCESS_SHIFT "              "

void nfc_scene_read_emv_app_success_dialog_callback(DialogExResult result, void* context) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_read_emv_app_success_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;

    // Setup view
    NfcDeviceData* data = &nfc->dev.dev_data;
    DialogEx* dialog_ex = nfc->dialog_ex;
    dialog_ex_set_left_button_text(dialog_ex, "Retry");
    dialog_ex_set_right_button_text(dialog_ex, "Run app");
    dialog_ex_set_header(
        dialog_ex, nfc->dev.dev_data.emv_data.name, 36, 8, AlignLeft, AlignCenter);
    dialog_ex_set_icon(dialog_ex, 8, 13, &I_Medium_chip_22x21);
    // Display UID
    nfc_text_store_set(
        nfc,
        NFC_SCENE_READ_SUCCESS_SHIFT "UID: %02X %02X %02X %02X \n",
        data->nfc_data.uid[0],
        data->nfc_data.uid[1],
        data->nfc_data.uid[2],
        data->nfc_data.uid[3]);
    dialog_ex_set_text(dialog_ex, nfc->text_store, 8, 16, AlignLeft, AlignTop);
    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_read_emv_app_success_dialog_callback);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

const bool nfc_scene_read_emv_app_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultLeft) {
            return scene_manager_previous_scene(nfc->scene_manager);
        } else if(event.event == DialogExResultRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRunEmvAppConfirm);
            return true;
        }
    }
    return false;
}

const void nfc_scene_read_emv_app_success_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    DialogEx* dialog_ex = nfc->dialog_ex;
    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignTop);
    dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
    dialog_ex_set_left_button_text(dialog_ex, NULL);
    dialog_ex_set_right_button_text(dialog_ex, NULL);
    dialog_ex_set_result_callback(dialog_ex, NULL);
    dialog_ex_set_context(dialog_ex, NULL);
}
