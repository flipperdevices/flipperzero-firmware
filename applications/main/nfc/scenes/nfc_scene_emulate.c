#include "../helpers/protocol_support/nfc_protocol_support.h"

#include "nfc_app_i.h"

void nfc_scene_emulate_timer_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(
        instance->view_dispatcher, NfcCustomEventEmulationTimeExpired);
}

void nfc_scene_emulate_on_enter(void* context) {
    nfc_protocol_support_on_enter(NfcProtocolSupportSceneEmulate, context);
}

bool nfc_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    return nfc_protocol_support_on_event(NfcProtocolSupportSceneEmulate, context, event);
}

void nfc_scene_emulate_on_exit(void* context) {
    nfc_protocol_support_on_exit(NfcProtocolSupportSceneEmulate, context);
}
