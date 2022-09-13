#include "../nfc_i.h"

void nfc_scene_not_init_popup_callback(void* context) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventViewExit);
}

void nfc_scene_not_init_on_enter(void* context) {
    Nfc* nfc = context;

    // Setup view
    Popup* popup = nfc->popup;
    popup_set_header(
        popup,
        "Error!\nNFC chip failed to start\n\n\nSend a photo of this to:\nsupport@flipperzero.one",
        0,
        0,
        AlignLeft,
        AlignTop);
    popup_set_context(popup, nfc);
    popup_set_timeout(popup, 30000);
    popup_enable_timeout(popup);
    popup_set_callback(popup, nfc_scene_not_init_popup_callback);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
}

bool nfc_scene_not_init_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    UNUSED(nfc);
    UNUSED(event);
    view_dispatcher_stop(nfc->view_dispatcher);

    return true;
}

void nfc_scene_not_init_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    popup_reset(nfc->popup);
}
