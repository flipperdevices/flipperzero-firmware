#include "../mfc_editor_app_i.h"

void mfc_editor_scene_data_view_on_enter(void* context) {
    MfcEditorApp* instance = context;

    DialogEx* dialog_ex = instance->dialog_ex;
    dialog_ex_set_context(instance->dialog_ex, instance);

    MfcEditorBlockView block_view =
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);

    const MfClassicData* mf_classic_data = instance->mf_classic_data;
    Iso14443_3aData* iso14443_3a_data = mf_classic_data->iso14443_3a_data;

    if(block_view == MfcEditorBlockViewUID) {
        dialog_ex_set_header(dialog_ex, "UID", 63, 3, AlignCenter, AlignTop);

        furi_string_set(instance->data_view_text, "");
        for(int i = 0; i < iso14443_3a_data->uid_len; i++) {
            furi_string_cat_printf(instance->data_view_text, "%02hhX ", iso14443_3a_data->uid[i]);
        }
        // Remove trailing space
        furi_string_trim(instance->data_view_text);
    } else if(block_view == MfcEditorBlockViewManufacturerBytes) {
        dialog_ex_set_header(dialog_ex, "Manufacturer Bytes", 63, 3, AlignCenter, AlignTop);

        if(mf_classic_is_block_read(mf_classic_data, 0)) {
            furi_string_set(instance->data_view_text, "");
            uint8_t byte_num = MF_CLASSIC_BLOCK_SIZE - iso14443_3a_data->uid_len - 1;
            // +1 to ignore BCC byte
            // TODO: should we do this for 4K?
            for(int i = iso14443_3a_data->uid_len + 1; i < MF_CLASSIC_BLOCK_SIZE; i++) {
                furi_string_cat_printf(
                    instance->data_view_text, "%02hhX", mf_classic_data->block[0].data[i]);
                // Go onto next line when halfway through
                if(MF_CLASSIC_BLOCK_SIZE - i - 1 == byte_num / 2) {
                    furi_string_cat(instance->data_view_text, "\n");
                } else {
                    furi_string_cat(instance->data_view_text, " ");
                }
            }
            // Remove trailing space
            furi_string_trim(instance->data_view_text);
        } else {
            furi_string_set(
                instance->data_view_text, "Data unavailable.\nBlock 0 has not been read.");
        }
    } else if(block_view == MfcEditorBlockViewKeyA) {
        dialog_ex_set_header(dialog_ex, "Key A", 63, 3, AlignCenter, AlignTop);
    } else if(block_view == MfcEditorBlockViewKeyB) {
        dialog_ex_set_header(dialog_ex, "Key B", 63, 3, AlignCenter, AlignTop);
    } else if(block_view == MfcEditorBlockViewAccessBits) {
        dialog_ex_set_header(dialog_ex, "Access Bits", 63, 3, AlignCenter, AlignTop);
    } else if(block_view == MfcEditorBlockViewUserByte) {
        dialog_ex_set_header(dialog_ex, "User Byte", 63, 3, AlignCenter, AlignTop);
    } else {
        furi_string_printf(instance->data_view_header, "Block %hhu Data", instance->current_block);
        dialog_ex_set_header(
            dialog_ex,
            furi_string_get_cstr(instance->data_view_header),
            63,
            3,
            AlignCenter,
            AlignTop);
    }

    dialog_ex_set_text(
        dialog_ex,
        furi_string_get_cstr(instance->data_view_text),
        63,
        31,
        AlignCenter,
        AlignCenter);

    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewDialogEx);
}

bool mfc_editor_scene_data_view_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    UNUSED(instance);
    UNUSED(event);

    return consumed;
}

void mfc_editor_scene_data_view_on_exit(void* context) {
    MfcEditorApp* instance = context;

    dialog_ex_reset(instance->dialog_ex);
}
