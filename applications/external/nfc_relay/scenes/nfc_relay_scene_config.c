#include "../nfc_relay.h"

void nfc_relay_scene_config_on_enter(void* context) {
    UNUSED(context);

    // Setup and start worker
}

bool nfc_relay_scene_config_on_event(void* context, SceneManagerEvent event) {
    NfcRelay* nfc_relay = context;
    bool consumed = false;
    UNUSED(nfc_relay);
    UNUSED(event);

    return consumed;
}

void nfc_relay_scene_config_on_exit(void* context) {
    NfcRelay* nfc_relay = context;
    UNUSED(nfc_relay);
    // Clear view
}