#include "../mfc_editor_app_i.h"

enum SubmenuIndex {
    // Reserve first indices for opening normal block
    SubmenuIndexReserved = MF_CLASSIC_TOTAL_BLOCKS_MAX,

    // Special options - Sector 0 only
    SubmenuIndexUID,
    SubmenuIndexManufacturerBytes,

    // Special options - All sectors
    SubmenuIndexKeyA,
    SubmenuIndexKeyB,
    SubmenuIndexAccessBits,
    SubmenuIndexUserByte,
};

void mfc_editor_scene_block_select_submenu_callback(void* context, uint32_t index) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void mfc_editor_scene_block_select_on_enter(void* context) {
    MfcEditorApp* instance = context;

    Submenu* submenu = instance->submenu;

    uint8_t first_block = mf_classic_get_first_block_num_of_sector(instance->current_sector);
    uint8_t block_num = mf_classic_get_blocks_num_in_sector(instance->current_sector);

    FuriString* label = furi_string_alloc();
    for(uint8_t i = 0; i < block_num; i++) {
        uint8_t block_index = first_block + i;
        furi_string_printf(label, "Block %hhu", block_index);
        submenu_add_item(
            submenu,
            furi_string_get_cstr(label),
            block_index,
            mfc_editor_scene_block_select_submenu_callback,
            instance);
    }
    furi_string_free(label);

    if(instance->current_sector == 0) {
        submenu_add_item(
            submenu,
            "UID",
            SubmenuIndexUID,
            mfc_editor_scene_block_select_submenu_callback,
            instance);
        submenu_add_item(
            submenu,
            "Manufacturer Bytes",
            SubmenuIndexManufacturerBytes,
            mfc_editor_scene_block_select_submenu_callback,
            instance);
    }

    submenu_add_item(
        submenu,
        "Key A",
        SubmenuIndexKeyA,
        mfc_editor_scene_block_select_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Key B",
        SubmenuIndexKeyB,
        mfc_editor_scene_block_select_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Access Bits",
        SubmenuIndexAccessBits,
        mfc_editor_scene_block_select_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "User Byte",
        SubmenuIndexUserByte,
        mfc_editor_scene_block_select_submenu_callback,
        instance);

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneBlockSelect));
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
}

bool mfc_editor_scene_block_select_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    UNUSED(instance);

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            instance->scene_manager, MfcEditorSceneBlockSelect, event.event);
        FURI_LOG_I(TAG, "Block select event %lu", event.event);
        consumed = true;
    }

    return consumed;
}

void mfc_editor_scene_block_select_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
