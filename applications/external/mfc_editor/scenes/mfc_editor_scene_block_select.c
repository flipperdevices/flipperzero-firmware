#include "../mfc_editor_app_i.h"

enum SubmenuIndex {
    // Reserve first indices for opening normal block
    SubmenuIndexReserved = MF_CLASSIC_TOTAL_BLOCKS_MAX,

    // Special options - Sector 0 only
    SubmenuIndexUID,
    SubmenuIndexBCC,
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
        furi_string_printf(label, "Block %u", block_index);
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
        if(instance->mf_classic_data->iso14443_3a_data->uid_len == 4) {
            // 7-byte UID cards don't store a BCC byte
            submenu_add_item(
                submenu,
                "BCC",
                SubmenuIndexBCC,
                mfc_editor_scene_block_select_submenu_callback,
                instance);
        }
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

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            instance->scene_manager, MfcEditorSceneBlockSelect, event.event);

        MfcEditorBlockView block_view;
        if(event.event == SubmenuIndexUID) {
            block_view = MfcEditorBlockViewUID;
        } else if(event.event == SubmenuIndexBCC) {
            block_view = MfcEditorBlockViewBCC;
        } else if(event.event == SubmenuIndexManufacturerBytes) {
            block_view = MfcEditorBlockViewManufacturerBytes;
        } else if(event.event == SubmenuIndexKeyA) {
            block_view = MfcEditorBlockViewKeyA;
        } else if(event.event == SubmenuIndexKeyB) {
            block_view = MfcEditorBlockViewKeyB;
        } else if(event.event == SubmenuIndexAccessBits) {
            block_view = MfcEditorBlockViewAccessBits;
            instance->current_block =
                mf_classic_get_first_block_num_of_sector(instance->current_sector);
        } else if(event.event == SubmenuIndexUserByte) {
            block_view = MfcEditorBlockViewUserByte;
        } else {
            block_view = MfcEditorBlockViewNormal;
            instance->current_block = event.event;
        }

        scene_manager_set_scene_state(instance->scene_manager, MfcEditorSceneDataView, block_view);
        scene_manager_next_scene(instance->scene_manager, MfcEditorSceneDataView);
        consumed = true;
    }

    return consumed;
}

void mfc_editor_scene_block_select_on_exit(void* context) {
    MfcEditorApp* instance = context;

    submenu_reset(instance->submenu);
}
