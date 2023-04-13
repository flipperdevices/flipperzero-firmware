#include "../tullave_i.h"

enum { TuLlaveStateCardSearch, TuLlaveStateCardFound };

bool tullave_check_worker_callback(TuLlaveWorkerEvent event, void* context) {
    furi_assert(context);

    TuLlave* t_llave = context;
    view_dispatcher_send_custom_event(t_llave->view_dispatcher, event);

    return true;
}

static void tullave_scene_check_setup_view(TuLlave* t_llave) {
    Popup* popup = t_llave->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(t_llave->scene_manager, TuLlaveSceneCheck);

    if(state == TuLlaveStateCardSearch) {
        popup_set_icon(t_llave->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            t_llave->popup, "Put TuLlave\ncard\n in the back", 128, 32, AlignRight, AlignCenter);
    }

    view_dispatcher_switch_to_view(t_llave->view_dispatcher, TuLlaveViewPopup);
}

bool tullave_scene_check_on_event(void* context, SceneManagerEvent event) {
    TuLlave* t_llave = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == TuLlaveWorkerEventCardDetected) {
            notification_message(t_llave->notifications, &seq_tullave_found);
            scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneReadSuccess);
            consumed = true;
        }
    }

    return consumed;
}

void tullave_scene_check_on_enter(void* context) {
    TuLlave* t_llave = context;

    scene_manager_set_scene_state(
        t_llave->scene_manager, TuLlaveSceneCheck, TuLlaveStateCardSearch);
    tullave_scene_check_setup_view(t_llave);

    tullave_worker_start(t_llave->worker, tullave_check_worker_callback, context);

    // Start blink notification
    notification_message(t_llave->notifications, &seq_search_tullave_blink);
}

void tullave_scene_check_on_exit(void* context) {
    TuLlave* t_llave = context;

    scene_manager_set_scene_state(
        t_llave->scene_manager, TuLlaveSceneCheck, TuLlaveStateCardSearch);
    // Clear view
    popup_reset(t_llave->popup);

    // Stop worker
    tullave_worker_stop(t_llave->worker);

    // Stop notification
    notification_message_block(t_llave->notifications, &sequence_reset_blue);
}
