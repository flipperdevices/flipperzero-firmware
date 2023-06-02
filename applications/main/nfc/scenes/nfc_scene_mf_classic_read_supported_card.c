#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_read_supported_card_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    popup_reset(nfc->popup);
    popup_set_text(nfc->popup, "Apply card to\nFlipper's back", 97, 24, AlignCenter, AlignTop);
    popup_set_icon(nfc->popup, 0, 8, &I_NFC_manual_60x50);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);



    nfc_blink_read_start(nfc);
}

bool nfc_scene_read_supported_card_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void nfc_scene_read_supported_card_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
