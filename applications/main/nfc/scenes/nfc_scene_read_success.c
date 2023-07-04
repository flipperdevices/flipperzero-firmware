#include "../nfc_app_i.h"

#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_read_success_on_enter(void* context) {
    NfcApp* nfc = context;
    nfc_protocol_support_build_scene_read_success(nfc);

    notification_message_block(nfc->notifications, &sequence_set_green_255);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = nfc_protocol_support_handle_scene_read_success(nfc, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_read_success_on_exit(void* context) {
    NfcApp* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);
    widget_reset(nfc->widget);
}
