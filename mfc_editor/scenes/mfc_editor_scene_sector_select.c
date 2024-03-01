#include "../mfc_editor_app_i.h"

void mfc_editor_scene_sector_select_submenu_callback(void* context, uint32_t index) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void mfc_editor_scene_sector_select_on_enter(void* context) {
    MfcEditorApp* instance = context;

    Submenu* submenu = instance->submenu;

    uint8_t sectors_num = mf_classic_get_total_sectors_num(instance->mf_classic_data->type);

    FuriString* label = furi_string_alloc();
    for(uint8_t i = 0; i < sectors_num; i++) {
        furi_string_printf(label, "Sector %u", i);
        submenu_add_item(
            submenu,
            furi_string_get_cstr(label),
            i,
            mfc_editor_scene_sector_select_submenu_callback,
            instance);
    }
    furi_string_free(label);

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneSectorSelect));
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
}

bool mfc_editor_scene_sector_select_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            instance->scene_manager, MfcEditorSceneSectorSelect, event.event);
        instance->current_sector = event.event;
        scene_manager_set_scene_state(instance->scene_manager, MfcEditorSceneBlockSelect, 0);
        scene_manager_next_scene(instance->scene_manager, MfcEditorSceneBlockSelect);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        if(instance->is_unsaved_changes) {
            MfcEditorSaveResponse response = mfc_editor_warn_unsaved_changes(instance);
            if(response == MfcEditorSaveResponseSave) {
                if(mfc_editor_save_file(instance)) {
                    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneSaveSuccess);
                }
                // Stop the scene manager from going back to previous scene
                consumed = true;
            } else if(response == MfcEditorSaveResponseCancel) {
                // Stop the scene manager from going back to previous scene
                consumed = true;
            }
        }
    }

    return consumed;
}

void mfc_editor_scene_sector_select_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
