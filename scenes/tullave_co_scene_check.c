#include "../tullave_co_i.h"

enum { TuLlaveStateCardSearch, TuLlaveStateCardFound };

bool tullave_check_worker_callback(TuLlaveWorkerEvent event, void* context) {
    furi_assert(context);

    TuLlave* t_llave = context;
    view_dispatcher_send_custom_event(t_llave->view_dispatcher, event);

    return true;
}

static void tullave_co_scene_check_setup_view(TuLlave* t_llave) {
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

bool tullave_co_scene_check_on_event(void* context, SceneManagerEvent event) {
    TuLlave* t_llave = context;
    bool consumed = false;

    //TODO: Consume event.

    UNUSED(t_llave);
    UNUSED(context);
    UNUSED(event);

    return consumed;
}

void tullave_co_scene_check_on_enter(void* context) {
    TuLlave* t_llave = context;

    scene_manager_set_scene_state(
        t_llave->scene_manager, TuLlaveSceneCheck, TuLlaveStateCardSearch);
    tullave_co_scene_check_setup_view(t_llave);

    tullave_worker_start(t_llave->worker, tullave_check_worker_callback, context);

    // Start blink notification
    tullave_blink_start(t_llave);
}

void tullave_co_scene_check_on_exit(void* context) {
    TuLlave* t_llave = context;

    //nfc_magic_worker_stop(nfc_magic->worker);
    scene_manager_set_scene_state(
        t_llave->scene_manager, TuLlaveSceneCheck, TuLlaveStateCardSearch);
    // Clear view
    popup_reset(t_llave->popup);

    // Stop worker
    tullave_worker_stop(t_llave->worker);

    // Stop notification
    tullave_blink_stop(t_llave);
}
