#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

static void
    nfc_crocodile_scene_transfer_done_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    NfcCrocodile* nfc_crocodile = context;

    if(result == DialogExResultLeft) {
        view_dispatcher_send_custom_event(nfc_crocodile->view_dispatcher, 0);
    } else if(result == DialogExResultRight) {
        view_dispatcher_send_custom_event(nfc_crocodile->view_dispatcher, 1);
    }
}

void nfc_crocodile_scene_transfer_done_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    dialog_ex_set_result_callback(
        nfc_crocodile->dialog, nfc_crocodile_scene_transfer_done_dialog_callback);
    dialog_ex_set_context(nfc_crocodile->dialog, context);
    dialog_ex_set_left_button_text(nfc_crocodile->dialog, "Retry");
    dialog_ex_set_right_button_text(nfc_crocodile->dialog, "Next word");
    dialog_ex_set_icon(nfc_crocodile->dialog, 8, 0, &I_RFIDDolphinSuccess_108x57);
    view_dispatcher_switch_to_view(nfc_crocodile->view_dispatcher, NfcCrocodileViewDialog);
}

bool nfc_crocodile_scene_transfer_done_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == 0) {
            scene_manager_previous_scene(nfc_crocodile->scene_manager);
            consumed = true;
        } else if(event.event == 1) {
            scene_manager_search_and_switch_to_previous_scene(
                nfc_crocodile->scene_manager, NfcCrocodileSceneDictionary);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_crocodile_scene_transfer_done_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    dialog_ex_reset(nfc_crocodile->dialog);
}