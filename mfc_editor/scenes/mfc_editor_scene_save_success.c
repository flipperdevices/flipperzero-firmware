#include "../mfc_editor_app_i.h"

void mfc_editor_scene_save_success_popup_callback(void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, MfcEditorCustomEventViewExit);
}

void mfc_editor_scene_save_success_on_enter(void* context) {
    MfcEditorApp* instance = context;

    Popup* popup = instance->popup;
    popup_set_icon(popup, 36, 5, &I_DolphinSaved_92x58);
    popup_set_header(popup, "Saved", 15, 19, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, instance);
    popup_set_callback(popup, mfc_editor_scene_save_success_popup_callback);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewPopup);
}

bool mfc_editor_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MfcEditorCustomEventViewExit) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, MfcEditorSceneFileSelect);
        }
    }

    return consumed;
}

void mfc_editor_scene_save_success_on_exit(void* context) {
    MfcEditorApp* instance = context;

    popup_reset(instance->popup);
}
