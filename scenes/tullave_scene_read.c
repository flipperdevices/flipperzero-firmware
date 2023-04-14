#include "../tullave_i.h"

typedef enum {
    TuLlaveSceneDetecting,
    TuLlaveSceneReading,
    TuLlaveSceneIdle
} TuLlaveSceneReadState;

bool tullave_read_worker_callback(TuLlaveWorkerEvent event, void* context) {
    furi_assert(context);

    TuLlave* t_llave = context;
    view_dispatcher_send_custom_event(t_llave->view_dispatcher, event);

    return true;
}

static void tullave_scene_read_set_state(TuLlave* t_llave, TuLlaveSceneReadState state) {
    Popup* popup = t_llave->popup;
    popup_reset(popup);

    scene_manager_set_scene_state(t_llave->scene_manager, TuLlaveSceneRead, state);

    if(state == TuLlaveSceneDetecting) {
        popup_set_icon(t_llave->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            t_llave->popup, "Put TuLlave\ncard\n in the back", 128, 32, AlignRight, AlignCenter);
    } else if(state == TuLlaveSceneReading) {
        popup_set_icon(t_llave->popup, 12, 23, &A_Loading_24);
        popup_set_text(
            t_llave->popup, "Reading TuLlave\nDon't move", 128, 32, AlignRight, AlignCenter);
    }

    view_dispatcher_switch_to_view(t_llave->view_dispatcher, TuLlaveViewPopup);
}

bool tullave_scene_read_on_event(void* context, SceneManagerEvent event) {
    TuLlave* t_llave = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == TuLlaveWorkerEventCardDetected) {
            tullave_scene_read_set_state(t_llave, TuLlaveSceneReading);
            notification_message(t_llave->notifications, &sequence_success);
            scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneReadSuccess);
            consumed = true;
        } else if(event.event == TuLLaveWorkerEventNoCardDetected) {
            tullave_scene_read_set_state(t_llave, TuLlaveSceneDetecting);
            notification_message(t_llave->notifications, &sequence_blink_start_blue);
            consumed = true;
        }
    }

    return consumed;
}

void tullave_scene_read_on_enter(void* context) {
    TuLlave* t_llave = context;

    notification_message(t_llave->notifications, &sequence_blink_start_blue);
    tullave_scene_read_set_state(t_llave, TuLlaveSceneDetecting);

    tullave_worker_start(t_llave->worker, tullave_read_worker_callback, context);
}

void tullave_scene_read_on_exit(void* context) {
    TuLlave* t_llave = context;

    // Clear view
    popup_reset(t_llave->popup);
    // Stop worker
    tullave_worker_stop(t_llave->worker);

    tullave_scene_read_set_state(t_llave, TuLlaveSceneIdle);
    notification_message(t_llave->notifications, &sequence_blink_stop);
}
