#include "../nfc_i.h"

#define NFC_SCENE_READ_SUCCESS_SHIFT "              "

void nfc_scene_delete_dialog_callback(DialogExResult result, void* context) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_delete_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;

    // Setup view
    NfcDeviceCommomData* data = (NfcDeviceCommomData*)&nfc->dev.dev_data.nfc_data;
    DialogEx* dialog_ex = nfc->dialog_ex;
    dialog_ex_set_left_button_text(dialog_ex, "Back");
    dialog_ex_set_right_button_text(dialog_ex, "Delete");
    dialog_ex_set_header(dialog_ex, nfc->dev.dev_name, 36, 8, AlignLeft, AlignCenter);
    // Display UID
    if(data->uid_len == 4) {
        nfc_text_store_set(
            nfc,
            NFC_SCENE_READ_SUCCESS_SHIFT "%s\n" NFC_SCENE_READ_SUCCESS_SHIFT
                                         "ATQA: %02X%02X SAK: %02X\nUID: %02X %02X %02X %02X",
            nfc_get_protocol(data->protocol),
            data->atqa[0],
            data->atqa[1],
            data->sak,
            data->uid[0],
            data->uid[1],
            data->uid[2],
            data->uid[3]);
    } else if(data->uid_len == 7) {
        nfc_text_store_set(
            nfc,
            NFC_SCENE_READ_SUCCESS_SHIFT
            "%s\n" NFC_SCENE_READ_SUCCESS_SHIFT
            "ATQA: %02X%02X SAK: %02X\nUID: %02X %02X %02X %02X %02X %02X %02X",
            nfc_get_protocol(data->protocol),
            data->atqa[0],
            data->atqa[1],
            data->sak,
            data->uid[0],
            data->uid[1],
            data->uid[2],
            data->uid[3],
            data->uid[4],
            data->uid[5],
            data->uid[6]);
    }
    dialog_ex_set_text(dialog_ex, nfc->text_store, 8, 16, AlignLeft, AlignTop);
    dialog_ex_set_context(dialog_ex, nfc);
    dialog_ex_set_result_callback(dialog_ex, nfc_scene_delete_dialog_callback);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

const bool nfc_scene_delete_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultLeft) {
            return scene_manager_previous_scene(nfc->scene_manager);
        } else if(event.event == DialogExResultRight) {
            if(nfc_device_delete(&nfc->dev)) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneDeleteSuccess);
            } else {
                scene_manager_search_previous_scene(nfc->scene_manager, NfcSceneStart);
            }
            return true;
        }
    }
    return false;
}

const void nfc_scene_delete_on_exit(void* context) {
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
