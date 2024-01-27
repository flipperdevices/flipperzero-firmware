#include "../mfc_editor_app_i.h"

void mfc_editor_scene_data_view_dialog_ex_callback(DialogExResult result, void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, result);
}

void mfc_editor_scene_data_view_update_display(MfcEditorApp* instance) {
    DialogEx* dialog_ex = instance->dialog_ex;

    dialog_ex_reset(instance->dialog_ex);
    dialog_ex_set_context(instance->dialog_ex, instance);
    dialog_ex_set_result_callback(
        instance->dialog_ex, mfc_editor_scene_data_view_dialog_ex_callback);

    MfcEditorBlockView block_view =
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);

    MfClassicData* mf_classic_data = instance->mf_classic_data;
    Iso14443_3aData* iso14443_3a_data = mf_classic_data->iso14443_3a_data;

    furi_string_reset(instance->data_view_text);

    if(block_view == MfcEditorBlockViewUID) {
        dialog_ex_set_header(dialog_ex, "UID", 63, 3, AlignCenter, AlignTop);

        mfc_editor_furi_string_render_bytes(
            instance->data_view_text, iso14443_3a_data->uid, iso14443_3a_data->uid_len);

        if(memcmp(
               iso14443_3a_data->uid, mf_classic_data->block[0].data, iso14443_3a_data->uid_len)) {
            // ISO-14443 UID does not match first bytes of block 0
            furi_string_cat_printf(instance->data_view_text, "\nBlock 0 does not match UID!\n(");
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text,
                mf_classic_data->block[0].data,
                iso14443_3a_data->uid_len);
            furi_string_push_back(instance->data_view_text, ')');
            dialog_ex_set_center_button_text(dialog_ex, "Fix");
        }

        if(mf_classic_is_block_read(mf_classic_data, 0)) {
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        }
    } else if(block_view == MfcEditorBlockViewBCC) {
        dialog_ex_set_header(dialog_ex, "Block Check Character", 63, 3, AlignCenter, AlignTop);

        uint8_t stored_bcc = mf_classic_data->block[0].data[4];
        uint8_t calculated_bcc =
            mfc_editor_calculate_uid_bcc(iso14443_3a_data->uid, iso14443_3a_data->uid_len);

        if(mf_classic_is_block_read(mf_classic_data, 0)) {
            furi_string_printf(
                instance->data_view_text,
                "Stored BCC: %02X\nActual BCC: %02X",
                stored_bcc,
                calculated_bcc);

            if(stored_bcc != calculated_bcc) {
                furi_string_cat(instance->data_view_text, "\n(Mismatch!)");
                dialog_ex_set_center_button_text(dialog_ex, "Fix");
            }
        } else {
            furi_string_printf(
                instance->data_view_text,
                "Actual BCC: %02X\nStored BCC is unavailable\nas Block 0 has not been read.",
                calculated_bcc);
        }
    } else if(block_view == MfcEditorBlockViewManufacturerBytes) {
        dialog_ex_set_header(dialog_ex, "Manufacturer Bytes", 63, 3, AlignCenter, AlignTop);

        if(mf_classic_is_block_read(mf_classic_data, 0)) {
            // Skip BCC byte (not present on 7B UID cards)
            bool skip_byte = iso14443_3a_data->uid_len == 4;
            uint8_t start_index = iso14443_3a_data->uid_len + skip_byte;
            uint8_t byte_num = MF_CLASSIC_BLOCK_SIZE - iso14443_3a_data->uid_len - skip_byte;
            uint8_t line_len = byte_num / 2;

            mfc_editor_furi_string_render_bytes(
                instance->data_view_text, mf_classic_data->block[0].data + start_index, line_len);
            furi_string_push_back(instance->data_view_text, '\n');
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text,
                mf_classic_data->block[0].data + start_index + line_len,
                byte_num - line_len);

            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            furi_string_set(
                instance->data_view_text, "Data unavailable.\nBlock 0 has not been read.");
        }
    } else if(block_view == MfcEditorBlockViewKeyA) {
        dialog_ex_set_header(dialog_ex, "Key A", 63, 3, AlignCenter, AlignTop);

        if(mf_classic_is_key_found(mf_classic_data, instance->current_sector, MfClassicKeyTypeA)) {
            MfClassicSectorTrailer* sector_trailer =
                mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector);
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text, sector_trailer->key_a.data, MF_CLASSIC_KEY_SIZE);
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            furi_string_set(
                instance->data_view_text, "Key A has not been found\nfor this sector.");
        }
    } else if(block_view == MfcEditorBlockViewKeyB) {
        dialog_ex_set_header(dialog_ex, "Key B", 63, 3, AlignCenter, AlignTop);

        if(mf_classic_is_key_found(mf_classic_data, instance->current_sector, MfClassicKeyTypeB)) {
            MfClassicSectorTrailer* sector_trailer =
                mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector);
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text, sector_trailer->key_b.data, MF_CLASSIC_KEY_SIZE);
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            furi_string_set(
                instance->data_view_text, "Key B has not been found\nfor this sector.");
        }
    } else if(block_view == MfcEditorBlockViewAccessBits) {
        uint8_t sector_trailer_num =
            mf_classic_get_sector_trailer_num_by_sector(instance->current_sector);

        if(mf_classic_is_block_read(mf_classic_data, sector_trailer_num)) {
            furi_string_printf(
                instance->data_view_header, "Access Bits (Block %u)", instance->current_block);
            dialog_ex_set_header(
                dialog_ex,
                furi_string_get_cstr(instance->data_view_header),
                63,
                3,
                AlignCenter,
                AlignTop);

            MfcEditorAccessBits access_bits =
                mfc_editor_get_block_access_bits(mf_classic_data, instance->current_block);

            furi_string_printf(
                instance->data_view_text,
                "C1: %i(%i), C2: %i(%i), C3: %i(%i)\n",
                FURI_BIT(access_bits.bits, 0),
                FURI_BIT(access_bits.check_bits, 0),
                FURI_BIT(access_bits.bits, 1),
                FURI_BIT(access_bits.check_bits, 1),
                FURI_BIT(access_bits.bits, 2),
                FURI_BIT(access_bits.check_bits, 2));

            if(access_bits.bits != access_bits.check_bits) {
                furi_string_cat(
                    instance->data_view_text,
                    "Access Bits are invalid.\nEntire sector inaccessible.");
            } else if(instance->current_block == sector_trailer_num) {
                furi_string_cat(
                    instance->data_view_text, access_sector_trailer_labels[access_bits.bits]);
            } else {
                furi_string_cat(
                    instance->data_view_text, access_data_block_labels[access_bits.bits]);
            }

            dialog_ex_set_center_button_text(dialog_ex, "Next");
            dialog_ex_set_left_button_text(dialog_ex, "Prev");
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            dialog_ex_set_header(dialog_ex, "Access Bits", 63, 3, AlignCenter, AlignTop);
            furi_string_printf(
                instance->data_view_text,
                "Access Bits unavailable.\nBlock %u has not been read.",
                sector_trailer_num);
        }
    } else if(block_view == MfcEditorBlockViewUserByte) {
        dialog_ex_set_header(dialog_ex, "User Byte", 63, 3, AlignCenter, AlignTop);

        uint8_t sector_trailer_num =
            mf_classic_get_sector_trailer_num_by_sector(instance->current_sector);

        if(mf_classic_is_block_read(mf_classic_data, sector_trailer_num)) {
            furi_string_printf(
                instance->data_view_text,
                "Free byte between\nAccess Bits and Key B:\n%02X",
                mf_classic_data->block[sector_trailer_num].data[9]);
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            furi_string_printf(
                instance->data_view_text,
                "Data unavailable.\nBlock %u has not been read.",
                sector_trailer_num);
        }
    } else {
        uint8_t current_block = instance->current_block;
        furi_string_printf(instance->data_view_header, "Block %u Data", current_block);
        dialog_ex_set_header(
            dialog_ex,
            furi_string_get_cstr(instance->data_view_header),
            63,
            3,
            AlignCenter,
            AlignTop);

        // Only display a block if it is fully read, and, if it is a sector trailer, both keys are found
        if(mf_classic_is_block_read(mf_classic_data, current_block) &&
           (!mf_classic_is_sector_trailer(current_block) ||
            (mf_classic_is_key_found(
                 mf_classic_data, instance->current_sector, MfClassicKeyTypeA) &&
             mf_classic_is_key_found(
                 mf_classic_data, instance->current_sector, MfClassicKeyTypeB)))) {
            // Split block data across 2 even lines
            const uint8_t* block_data = mf_classic_data->block[current_block].data;
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text, block_data, MF_CLASSIC_BLOCK_SIZE / 2);
            furi_string_push_back(instance->data_view_text, '\n');
            mfc_editor_furi_string_render_bytes(
                instance->data_view_text,
                block_data + MF_CLASSIC_BLOCK_SIZE / 2,
                MF_CLASSIC_BLOCK_SIZE / 2);
            dialog_ex_set_right_button_text(dialog_ex, "Edit");
        } else {
            furi_string_set(
                instance->data_view_text, "Data unavailable.\nBlock has not been fully read.");
        }
    }

    dialog_ex_set_text(
        dialog_ex,
        furi_string_get_cstr(instance->data_view_text),
        63,
        31,
        AlignCenter,
        AlignCenter);
}

void mfc_editor_scene_data_view_on_enter(void* context) {
    MfcEditorApp* instance = context;

    mfc_editor_scene_data_view_update_display(instance);

    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewDialogEx);
}

bool mfc_editor_scene_data_view_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        MfcEditorBlockView block_view =
            scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);
        if(block_view == MfcEditorBlockViewNormal) {
            if(event.event == DialogExResultRight) {
                // Block 0 and sector trailer blocks are risky edits
                bool risky_block =
                    instance->current_block == 0 ||
                    instance->current_block ==
                        mf_classic_get_sector_trailer_num_by_block(instance->current_block);
                if(!risky_block || mfc_editor_warn_risky_operation(instance)) {
                    scene_manager_set_scene_state(
                        instance->scene_manager, MfcEditorSceneDataEdit, block_view);
                    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneDataEdit);
                }
                consumed = true;
            }
        } else if(block_view == MfcEditorBlockViewUID) {
            if(event.event == DialogExResultRight) {
                if(mfc_editor_warn_risky_operation(instance)) {
                    scene_manager_set_scene_state(
                        instance->scene_manager, MfcEditorSceneDataEdit, block_view);
                    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneDataEdit);
                }
                consumed = true;
            } else if(event.event == DialogExResultCenter) {
                if(mfc_editor_warn_risky_operation(instance)) {
                    memcpy(
                        instance->mf_classic_data->block[0].data,
                        instance->mf_classic_data->iso14443_3a_data->uid,
                        instance->mf_classic_data->iso14443_3a_data->uid_len);
                    instance->is_unsaved_changes = true;
                    mfc_editor_scene_data_view_update_display(instance);
                }
                consumed = true;
            }
        } else if(
            block_view == MfcEditorBlockViewManufacturerBytes ||
            block_view == MfcEditorBlockViewKeyA || block_view == MfcEditorBlockViewKeyB) {
            if(event.event == DialogExResultRight) {
                if(mfc_editor_warn_risky_operation(instance)) {
                    scene_manager_set_scene_state(
                        instance->scene_manager, MfcEditorSceneDataEdit, block_view);
                    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneDataEdit);
                }
                consumed = true;
            }
        } else if(block_view == MfcEditorBlockViewBCC) {
            if(event.event == DialogExResultCenter) {
                if(mfc_editor_warn_risky_operation(instance)) {
                    // Fix BCC byte by setting it to calculated one
                    instance->mf_classic_data->block[0].data[4] = mfc_editor_calculate_uid_bcc(
                        instance->mf_classic_data->iso14443_3a_data->uid,
                        instance->mf_classic_data->iso14443_3a_data->uid_len);
                    instance->is_unsaved_changes = true;
                    mfc_editor_scene_data_view_update_display(instance);
                }
                consumed = true;
            }
        } else if(block_view == MfcEditorBlockViewAccessBits) {
            if(event.event == DialogExResultLeft) {
                uint8_t new_sector = mf_classic_get_sector_by_block(--instance->current_block);
                if(new_sector != instance->current_sector) {
                    instance->current_block =
                        mf_classic_get_sector_trailer_num_by_sector(instance->current_sector);
                }
                mfc_editor_scene_data_view_update_display(instance);
                consumed = true;
            } else if(event.event == DialogExResultCenter) {
                uint8_t new_sector = mf_classic_get_sector_by_block(++instance->current_block);
                if(new_sector != instance->current_sector) {
                    instance->current_block =
                        mf_classic_get_first_block_num_of_sector(instance->current_sector);
                }
                mfc_editor_scene_data_view_update_display(instance);
                consumed = true;
            } else if(event.event == DialogExResultRight) {
                if(mfc_editor_warn_risky_operation(instance)) {
                    scene_manager_next_scene(
                        instance->scene_manager, MfcEditorSceneDataEditAccessBits);
                }
                consumed = true;
            }
        } else if(block_view == MfcEditorBlockViewUserByte) {
            scene_manager_set_scene_state(
                instance->scene_manager, MfcEditorSceneDataEdit, block_view);
            scene_manager_next_scene(instance->scene_manager, MfcEditorSceneDataEdit);
            consumed = true;
        }
    }

    return consumed;
}

void mfc_editor_scene_data_view_on_exit(void* context) {
    MfcEditorApp* instance = context;

    dialog_ex_reset(instance->dialog_ex);
}
