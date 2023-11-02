#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

static void nfc_crocodile_scene_about_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    NfcCrocodile* nfc_crocodile = context;

    if(result == DialogExResultLeft) {
        view_dispatcher_send_custom_event(nfc_crocodile->view_dispatcher, 0);
    } else if(result == DialogExResultRight) {
        view_dispatcher_send_custom_event(nfc_crocodile->view_dispatcher, 1);
    }
}

void nfc_crocodile_scene_about_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    dialog_ex_set_result_callback(
        nfc_crocodile->dialog, nfc_crocodile_scene_about_dialog_callback);
    dialog_ex_set_context(nfc_crocodile->dialog, context);
    dialog_ex_set_left_button_text(nfc_crocodile->dialog, "Back");
    dialog_ex_set_text(nfc_crocodile->dialog, "(c) f1remoon", 64, 11, AlignCenter, AlignTop);
    view_dispatcher_switch_to_view(nfc_crocodile->view_dispatcher, NfcCrocodileViewDialog);
}

bool nfc_crocodile_scene_about_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == 0) {
            scene_manager_previous_scene(nfc_crocodile->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_crocodile_scene_about_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    dialog_ex_reset(nfc_crocodile->dialog);
}