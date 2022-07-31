#include "../nfc_i.h"

bool nfc_scene_mf_ultralight_auth_worker_callback(NfcWorkerEvent event, void* context) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
    return true;
}

void nfc_scene_mf_ultralight_auth_on_enter(void* context) {
    Nfc* nfc = context;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_context(dialog_ex, nfc);

    dialog_ex_set_header(dialog_ex, "Attacking...", 64, 10, AlignCenter, AlignCenter);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);

    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateMfUltralightAuthAttack,
        &nfc->dev->dev_data,
        nfc_scene_mf_ultralight_auth_worker_callback,
        nfc);
}

bool nfc_scene_mf_ultralight_auth_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    } else if(event.type == SceneManagerEventTypeCustom) {
        DialogEx* dialog_ex = nfc->dialog_ex;
        if(event.event == NfcWorkerEventNewAlgo) {
            consumed = true;
        } else if(event.event == NfcWorkerEventSuccess) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventFail) {
            dialog_ex_set_header(dialog_ex, "Key not found :(", 64, 10, AlignCenter, AlignCenter);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_mf_ultralight_auth_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    submenu_reset(nfc->submenu);
}
