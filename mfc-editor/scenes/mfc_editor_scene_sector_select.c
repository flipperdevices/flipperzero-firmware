#include "../mfc_editor_app_i.h"

void mfc_editor_scene_sector_select_submenu_callback(void* context, uint32_t index) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void mfc_editor_scene_sector_select_on_enter(void* context) {
    MfcEditorApp* instance = context;

    const MfClassicData* mf_classic_data =
        nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);

    Submenu* submenu = instance->submenu;
    submenu_set_header(submenu, "Select sector");

    for(int i = 0; i < mf_classic_get_total_sectors_num(mf_classic_data->type); i++) {
        char label[sizeof("Sector ") + 20];
        snprintf(label, sizeof(label), "Sector %i", i);
        submenu_add_item(
            submenu, label, i, mfc_editor_scene_sector_select_submenu_callback, instance);
    }

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneSectorSelect));
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
}

bool mfc_editor_scene_sector_select_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_I(TAG, "Opened sector %lu", event.event);
        scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, MfcEditorSceneStart);
    }

    return consumed;
}

void mfc_editor_scene_sector_select_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
