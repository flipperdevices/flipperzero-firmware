#include "../nfc_app_i.h"

#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_info_on_enter(void* context) {
    NfcApp* nfc = context;

    nfc_protocol_support_build_scene_info(nfc);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_info_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = nfc_protocol_support_handle_scene_info(nfc, event.event);
    }

    return consumed;
}

void nfc_scene_info_on_exit(void* context) {
    NfcApp* nfc = context;
    widget_reset(nfc->widget);
}
