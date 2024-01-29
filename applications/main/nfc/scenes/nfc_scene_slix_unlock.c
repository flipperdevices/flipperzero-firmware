#include "../nfc_app_i.h"

void nfc_scene_slix_unlock_on_enter(void* context) {
    NfcApp* instance = context;

    popup_set_header(instance->popup, "Hello motor", 64, 32, AlignCenter, AlignCenter);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);
}

bool nfc_scene_slix_unlock_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;
    UNUSED(instance);
    UNUSED(event);

    return consumed;
}

void nfc_scene_slix_unlock_on_exit(void* context) {
    NfcApp* instance = context;

    popup_reset(instance->popup);
}
