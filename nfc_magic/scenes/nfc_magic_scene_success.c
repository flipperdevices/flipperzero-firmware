#include "../nfc_magic_app_i.h"

void nfc_magic_scene_success_popup_callback(void* context) {
    NfcMagicApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcMagicAppCustomEventViewExit);
}

void nfc_magic_scene_success_on_enter(void* context) {
    NfcMagicApp* instance = context;

    notification_message(instance->notifications, &sequence_success);

    Popup* popup = instance->popup;
    popup_set_icon(popup, 36, 5, &I_DolphinDone_80x58);
    popup_set_header(popup, "Success!", 10, 20, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, instance);
    popup_set_callback(popup, nfc_magic_scene_success_popup_callback);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewPopup);
}

bool nfc_magic_scene_success_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcMagicAppCustomEventViewExit) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcMagicSceneStart);
        }
    }
    return consumed;
}

void nfc_magic_scene_success_on_exit(void* context) {
    NfcMagicApp* instance = context;

    // Clear view
    popup_reset(instance->popup);
}
