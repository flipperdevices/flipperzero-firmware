#include "../nfc_magic_i.h"

void nfc_magic_scene_wrong_card_on_enter(void* context) {
    UNUSED(context);
    // NfcMagic* nfc_magic = context;

    // view_dispatcher_switch_to_view(nfc_magic->view_dispatcher, NfcMagicViewWrongCard);
}

bool nfc_magic_scene_wrong_card_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_magic_scene_wrong_card_on_exit(void* context) {
    UNUSED(context);
}
