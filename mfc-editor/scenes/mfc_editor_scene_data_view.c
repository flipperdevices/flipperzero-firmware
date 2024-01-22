#include "../mfc_editor_app_i.h"

const char* access_data_block_labels[8] = {
    // C3, C2, C1
    "Key A: Read, Write, Inc, Dec\nKey B: Read, Write, Inc, Dec", // 000
    "Key A: Read\nKey B: Read, Write", // 001
    "Key A: Read\nKey B: Read", // 010
    "Key A: Read, Dec\nKey B: Read, Write, Inc, Dec", // 011
    "Key A: Read, Dec\nKey B: Read, Dec", // 100
    "Key A: No Access\nKey B: Read", // 101
    "Key A: No Access\nKey B: Read, Write", // 110
    "Key A: No Access\nKey B: No Access", // 111
};

const char* access_sector_trailer_labels[8] = {
    // C3, C2, C1
    "Key A: KA-W, AB-R, KB-RW\nKey B: No Access", // 000
    "Key A: AB-R\nKey B: KA+KB-W, AB-R", // 001
    "Key A: AB+KB-R\nKey B: No Access", // 010
    "Key A: AB-R\nKey B: AB-R", // 011
    "Key A: KA-W, AB+KB-RW\nKey B: No Access", // 100
    "Key A: AB-R\nKey B: AB-RW", // 101
    "Key A: AB-R\nKey B: KA+KB-W, AB-RW", // 110
    "Key A: AB-R\nKey B: AB-R", // 111
};

void mfc_editor_scene_data_view_update_display(MfcEditorApp* instance) {
    DialogEx* dialog_ex = instance->dialog_ex;

    MfcEditorBlockView block_view =
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);

    MfClassicData* mf_classic_data = instance->mf_classic_data;
    Iso14443_3aData* iso14443_3a_data = mf_classic_data->iso14443_3a_data;

    furi_string_reset(instance->data_view_text);

    if(block_view == MfcEditorBlockViewUID) {
        dialog_ex_set_header(dialog_ex, "UID", 63, 3, AlignCenter, AlignTop);

        for(int i = 0; i < iso14443_3a_data->uid_len; i++) {
            furi_string_cat_printf(instance->data_view_text, "%02X ", iso14443_3a_data->uid[i]);
        }
        // Remove trailing space
        furi_string_trim(instance->data_view_text);
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
            uint8_t byte_num = MF_CLASSIC_BLOCK_SIZE - iso14443_3a_data->uid_len - skip_byte;
            for(int i = iso14443_3a_data->uid_len + skip_byte; i < MF_CLASSIC_BLOCK_SIZE; i++) {
                furi_string_cat_printf(
                    instance->data_view_text, "%02X", mf_classic_data->block[0].data[i]);
                // Go onto next line when halfway through
                if(MF_CLASSIC_BLOCK_SIZE - i - 1 == byte_num / 2) {
                    furi_string_push_back(instance->data_view_text, '\n');
                } else {
                    furi_string_push_back(instance->data_view_text, ' ');
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

        if(mf_classic_is_key_found(mf_classic_data, instance->current_sector, MfClassicKeyTypeA)) {
            MfClassicSectorTrailer* sector_trailer =
                mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector);
            MfClassicKey key_a = sector_trailer->key_a;
            furi_string_printf(
                instance->data_view_text,
                "%02X %02X %02X %02X %02X %02X",
                key_a.data[0],
                key_a.data[1],
                key_a.data[2],
                key_a.data[3],
                key_a.data[4],
                key_a.data[5]);
        } else {
            furi_string_set(
                instance->data_view_text, "Key A has not been found\nfor this sector.");
        }
    } else if(block_view == MfcEditorBlockViewKeyB) {
        dialog_ex_set_header(dialog_ex, "Key B", 63, 3, AlignCenter, AlignTop);

        if(mf_classic_is_key_found(mf_classic_data, instance->current_sector, MfClassicKeyTypeB)) {
            MfClassicSectorTrailer* sector_trailer =
                mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector);
            MfClassicKey key_b = sector_trailer->key_b;
            furi_string_printf(
                instance->data_view_text,
                "%02X %02X %02X %02X %02X %02X",
                key_b.data[0],
                key_b.data[1],
                key_b.data[2],
                key_b.data[3],
                key_b.data[4],
                key_b.data[5]);
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
                    "Access Bits are invalid.\nNo operations are allowed.");
            } else if(instance->current_block == sector_trailer_num) {
                furi_string_cat(
                    instance->data_view_text, access_sector_trailer_labels[access_bits.bits]);
            } else {
                furi_string_cat(
                    instance->data_view_text, access_data_block_labels[access_bits.bits]);
            }

            dialog_ex_set_center_button_text(dialog_ex, "Next");
            dialog_ex_set_left_button_text(dialog_ex, "Prev");
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
            for(int i = 0; i < MF_CLASSIC_BLOCK_SIZE / 2; i++) {
                furi_string_cat_printf(instance->data_view_text, "%02X ", block_data[i]);
            }
            // Remove trailing space
            furi_string_trim(instance->data_view_text);
            furi_string_push_back(instance->data_view_text, '\n');
            for(int i = MF_CLASSIC_BLOCK_SIZE / 2; i < MF_CLASSIC_BLOCK_SIZE; i++) {
                furi_string_cat_printf(instance->data_view_text, "%02X ", block_data[i]);
            }
            // Remove trailing space
            furi_string_trim(instance->data_view_text);
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

void mfc_editor_scene_data_view_dialog_ex_callback(DialogExResult result, void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, result);
}

void mfc_editor_scene_data_view_on_enter(void* context) {
    MfcEditorApp* instance = context;

    dialog_ex_set_context(instance->dialog_ex, instance);

    mfc_editor_scene_data_view_update_display(instance);

    dialog_ex_set_result_callback(
        instance->dialog_ex, mfc_editor_scene_data_view_dialog_ex_callback);
    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewDialogEx);
}

bool mfc_editor_scene_data_view_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        MfcEditorBlockView block_view =
            scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);
        if(block_view == MfcEditorBlockViewAccessBits) {
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
            }
        }
    }

    return consumed;
}

void mfc_editor_scene_data_view_on_exit(void* context) {
    MfcEditorApp* instance = context;

    dialog_ex_reset(instance->dialog_ex);
}
