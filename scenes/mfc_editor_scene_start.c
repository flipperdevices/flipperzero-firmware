#include "../mfc_editor_app_i.h"

enum SubmenuIndex {
    SubmenuIndexOpen,
    SubmenuIndexCreate,
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
    submenu_add_item(
        submenu,
        "Create New",
        SubmenuIndexCreate,
        mfc_editor_scene_start_submenu_callback,
        instance);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneStart));
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
}

bool mfc_editor_scene_start_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexOpen) {
            scene_manager_next_scene(instance->scene_manager, MfcEditorSceneFileSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexCreate) {
            DialogMessage* message = dialog_message_alloc();
            dialog_message_set_header(
                message, "Not implemented", 63, 30, AlignCenter, AlignCenter);
            dialog_message_show(instance->dialogs, message);
            dialog_message_free(message);
            consumed = true;
        }
    }

    if(consumed) {
        scene_manager_set_scene_state(instance->scene_manager, MfcEditorSceneStart, event.event);
    }

    return consumed;
}

void mfc_editor_scene_start_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
