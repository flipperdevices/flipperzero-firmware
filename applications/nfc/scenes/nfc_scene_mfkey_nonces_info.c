#include "../nfc_i.h"

void nfc_scene_mfkey_nonces_info_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_mfkey_nonces_info_on_enter(void* context) {
    Nfc* nfc = context;

    widget_add_string_element(nfc->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Nonces saved");
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);

    nfc_blink_start(nfc);
}

bool nfc_scene_mfkey_nonces_info_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveSuccess);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_mfkey_nonces_info_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    widget_reset(nfc->widget);

    nfc_blink_stop(nfc);
}
