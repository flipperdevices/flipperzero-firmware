#include "../mfc_editor_app_i.h"

void mfc_editor_scene_invalid_file_popup_callback(void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, MfcEditorCustomEventViewExit);
}

void mfc_editor_scene_invalid_file_on_enter(void* context) {
    MfcEditorApp* instance = context;

    Popup* popup = instance->popup;
    popup_set_header(popup, "Invalid file", 63, 10, AlignCenter, AlignTop);
    popup_set_text(
        popup, "Only MIFARE Classic files\nare supported", 63, 40, AlignCenter, AlignCenter);
    popup_set_context(popup, instance);
    popup_set_callback(popup, mfc_editor_scene_invalid_file_popup_callback);

    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewPopup);
}

bool mfc_editor_scene_invalid_file_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MfcEditorCustomEventViewExit) {
            consumed = scene_manager_previous_scene(instance->scene_manager);
        }
    }

    return consumed;
}

void mfc_editor_scene_invalid_file_on_exit(void* context) {
    MfcEditorApp* instance = context;

    popup_reset(instance->popup);
}
