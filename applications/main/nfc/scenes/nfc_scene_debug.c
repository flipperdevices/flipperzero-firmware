#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_debug_on_enter(void* context) {
    nfc_protocol_support_on_enter(NfcProtocolSupportSceneDebug, context);
}

bool nfc_scene_debug_on_event(void* context, SceneManagerEvent event) {
    return nfc_protocol_support_on_event(NfcProtocolSupportSceneDebug, context, event);
}

void nfc_scene_debug_on_exit(void* context) {
    nfc_protocol_support_on_exit(NfcProtocolSupportSceneDebug, context);
}
