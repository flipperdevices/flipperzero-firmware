#include "../mfc_editor_app_i.h"

void mfc_editor_scene_data_edit_access_bits_dialog_ex_callback(
    DialogExResult result,
    void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, result);
}

void mfc_editor_scene_data_edit_access_bits_update_display(MfcEditorApp* instance) {
    DialogEx* dialog_ex = instance->dialog_ex;

    uint8_t sector_trailer_num =
        mf_classic_get_sector_trailer_num_by_sector(instance->current_sector);

    furi_string_printf(
        instance->data_view_text,
        "C1: %i, C2: %i, C3: %i\n",
        FURI_BIT(instance->access_bits_edit.bits, 0),
        FURI_BIT(instance->access_bits_edit.bits, 1),
        FURI_BIT(instance->access_bits_edit.bits, 2));

    if(instance->current_block == sector_trailer_num) {
        furi_string_cat(
            instance->data_view_text,
            access_sector_trailer_labels[instance->access_bits_edit.bits]);
    } else {
        furi_string_cat(
            instance->data_view_text, access_data_block_labels[instance->access_bits_edit.bits]);
    }

    dialog_ex_set_text(
        dialog_ex,
        furi_string_get_cstr(instance->data_view_text),
        63,
        31,
        AlignCenter,
        AlignCenter);
}

void mfc_editor_scene_data_edit_access_bits_on_enter(void* context) {
    MfcEditorApp* instance = context;

    DialogEx* dialog_ex = instance->dialog_ex;

    dialog_ex_set_context(instance->dialog_ex, instance);

    if(instance->current_sector >= 32 && !mf_classic_is_sector_trailer(instance->current_block)) {
        // 4K large sector - access bits affect range of blocks
        uint8_t sector_start_num =
            mf_classic_get_first_block_num_of_sector(instance->current_sector);
        // Relative to the sector start, round down to multiple of 5
        uint8_t relative_block_num = (instance->current_block - sector_start_num) / 5 * 5;
        furi_string_printf(
            instance->data_view_header,
            "Edit B. %u-%u Access",
            sector_start_num + relative_block_num,
            sector_start_num + relative_block_num + 4);
    } else {
        furi_string_printf(
            instance->data_view_header, "Edit Block %u Access", instance->current_block);
    }

    dialog_ex_set_header(
        dialog_ex, furi_string_get_cstr(instance->data_view_header), 63, 3, AlignCenter, AlignTop);

    instance->access_bits_edit =
        mfc_editor_get_block_access_bits(instance->mf_classic_data, instance->current_block);

    if(instance->access_bits_edit.bits != instance->access_bits_edit.check_bits) {
        // If access check bits don't match, make them match now
        instance->access_bits_edit.check_bits = instance->access_bits_edit.bits;
    }

    mfc_editor_scene_data_edit_access_bits_update_display(instance);

    dialog_ex_set_center_button_text(dialog_ex, "Save");
    dialog_ex_set_left_button_text(dialog_ex, "Prev");
    dialog_ex_set_right_button_text(dialog_ex, "Next");

    dialog_ex_set_result_callback(
        dialog_ex, mfc_editor_scene_data_edit_access_bits_dialog_ex_callback);
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewDialogEx);
}

bool mfc_editor_scene_data_edit_access_bits_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultLeft) {
            instance->access_bits_edit.bits--;
            instance->access_bits_edit.check_bits = instance->access_bits_edit.bits;
            mfc_editor_scene_data_edit_access_bits_update_display(instance);
            consumed = true;
        } else if(event.event == DialogExResultRight) {
            instance->access_bits_edit.bits++;
            instance->access_bits_edit.check_bits = instance->access_bits_edit.bits;
            mfc_editor_scene_data_edit_access_bits_update_display(instance);
            consumed = true;
        } else if(event.event == DialogExResultCenter) {
            mfc_editor_set_block_access_bits(
                instance->mf_classic_data, instance->current_block, &instance->access_bits_edit);
            instance->is_unsaved_changes = true;
            scene_manager_previous_scene(instance->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void mfc_editor_scene_data_edit_access_bits_on_exit(void* context) {
    MfcEditorApp* instance = context;

    dialog_ex_reset(instance->dialog_ex);
}
