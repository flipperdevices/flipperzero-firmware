#include <furi.h>

#include "../nfc_app_i.h"
#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_card_dump_on_enter(void* context) {
    nfc_protocol_support_on_enter(NfcProtocolSupportSceneCardDump, context);
}

bool nfc_scene_card_dump_on_event(void* context, SceneManagerEvent event) {
    return nfc_protocol_support_on_event(NfcProtocolSupportSceneCardDump, context, event);
}

void nfc_scene_card_dump_on_exit(void* context) {
    nfc_protocol_support_on_exit(NfcProtocolSupportSceneCardDump, context);
}