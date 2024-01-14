#include "../mfc_editor_app_i.h"

enum SubmenuIndex {
    SubmenuIndexOpen,
};

void mfc_editor_scene_start_submenu_callback(void* context, uint32_t index) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void mfc_editor_scene_start_on_enter(void* context) {
    MfcEditorApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu, "Open", SubmenuIndexOpen, mfc_editor_scene_start_submenu_callback, instance);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneStart));
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
}

bool mfc_editor_scene_start_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexOpen) {
            FURI_LOG_I(TAG, "Open item clicked");
            scene_manager_set_scene_state(
                instance->scene_manager, MfcEditorSceneStart, SubmenuIndexOpen);
            consumed = true;
        }
    }

    return consumed;
}

void mfc_editor_scene_start_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
