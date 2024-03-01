#include "../mfc_editor_app_i.h"

void mfc_editor_scene_data_edit_block_view_save_callback(void* context) {
    MfcEditorApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, MfcEditorCustomEventSave);
}

void mfc_editor_scene_data_edit_block_view_changed_callback(void* context) {
    UNUSED(context);
}

void mfc_editor_scene_data_edit_on_enter(void* context) {
    MfcEditorApp* instance = context;

    ByteInput* byte_input = instance->byte_input;

    MfClassicData* mf_classic_data = instance->mf_classic_data;
    Iso14443_3aData* iso14443_3a_data = mf_classic_data->iso14443_3a_data;

    MfcEditorBlockView block_view =
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);

    if(block_view == MfcEditorBlockViewUID) {
        byte_input_set_header_text(byte_input, "Enter new UID");

        instance->edit_buffer = malloc(iso14443_3a_data->uid_len);
        memcpy(instance->edit_buffer, iso14443_3a_data->uid, iso14443_3a_data->uid_len);

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            iso14443_3a_data->uid_len);
    } else if(block_view == MfcEditorBlockViewManufacturerBytes) {
        byte_input_set_header_text(byte_input, "Enter new Manufacturer Bytes");

        // Skip BCC byte (not present on 7B UID cards)
        bool skip_byte = iso14443_3a_data->uid_len == 4;
        uint8_t byte_num = MF_CLASSIC_BLOCK_SIZE - iso14443_3a_data->uid_len - skip_byte;

        instance->edit_buffer = malloc(byte_num);
        memcpy(
            instance->edit_buffer,
            mf_classic_data->block[0].data + iso14443_3a_data->uid_len + skip_byte,
            byte_num);

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            byte_num);
    } else if(block_view == MfcEditorBlockViewKeyA) {
        byte_input_set_header_text(byte_input, "Enter new Key A");

        instance->edit_buffer = malloc(MF_CLASSIC_KEY_SIZE);
        memcpy(
            instance->edit_buffer,
            mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector)
                ->key_a.data,
            MF_CLASSIC_KEY_SIZE);

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            MF_CLASSIC_KEY_SIZE);
    } else if(block_view == MfcEditorBlockViewKeyB) {
        byte_input_set_header_text(byte_input, "Enter new Key B");

        instance->edit_buffer = malloc(MF_CLASSIC_KEY_SIZE);
        memcpy(
            instance->edit_buffer,
            mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector)
                ->key_b.data,
            MF_CLASSIC_KEY_SIZE);

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            MF_CLASSIC_KEY_SIZE);
    } else if(block_view == MfcEditorBlockViewUserByte) {
        byte_input_set_header_text(byte_input, "Enter new User Byte");

        instance->edit_buffer = malloc(1);
        instance->edit_buffer[0] =
            mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector)
                ->access_bits.data[3];

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            1);
    } else {
        byte_input_set_header_text(byte_input, "Enter new block content");

        instance->edit_buffer = malloc(MF_CLASSIC_BLOCK_SIZE);
        memcpy(
            instance->edit_buffer,
            mf_classic_data->block[instance->current_block].data,
            MF_CLASSIC_BLOCK_SIZE);

        byte_input_set_result_callback(
            byte_input,
            mfc_editor_scene_data_edit_block_view_save_callback,
            mfc_editor_scene_data_edit_block_view_changed_callback,
            instance,
            instance->edit_buffer,
            MF_CLASSIC_BLOCK_SIZE);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, MfcEditorAppViewByteInput);
}

bool mfc_editor_scene_data_edit_on_event(void* context, SceneManagerEvent event) {
    MfcEditorApp* instance = context;
    bool consumed = false;

    MfClassicData* mf_classic_data = instance->mf_classic_data;
    Iso14443_3aData* iso14443_3a_data = mf_classic_data->iso14443_3a_data;

    MfcEditorBlockView block_view =
        scene_manager_get_scene_state(instance->scene_manager, MfcEditorSceneDataView);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MfcEditorCustomEventSave) {
            if(block_view == MfcEditorBlockViewNormal) {
                memcpy(
                    mf_classic_data->block[instance->current_block].data,
                    instance->edit_buffer,
                    MF_CLASSIC_BLOCK_SIZE);
                if(instance->current_block == 0) {
                    // UID needs to be equal to the first bytes in block 0
                    memcpy(
                        iso14443_3a_data->uid,
                        mf_classic_data->block[0].data,
                        iso14443_3a_data->uid_len);
                }
            } else if(block_view == MfcEditorBlockViewUID) {
                memcpy(iso14443_3a_data->uid, instance->edit_buffer, iso14443_3a_data->uid_len);
                // UID is also the first bytes in block 0
                memcpy(
                    mf_classic_data->block[0].data,
                    iso14443_3a_data->uid,
                    iso14443_3a_data->uid_len);
                // 4-byte UID cards need the BCC byte set to correct value
                if(iso14443_3a_data->uid_len == 4) {
                    mf_classic_data->block[0].data[4] = mfc_editor_calculate_uid_bcc(
                        iso14443_3a_data->uid, iso14443_3a_data->uid_len);
                }
            } else if(block_view == MfcEditorBlockViewManufacturerBytes) {
                // Skip BCC byte (not present on 7B UID cards)
                bool skip_byte = iso14443_3a_data->uid_len == 4;
                uint8_t byte_num = MF_CLASSIC_BLOCK_SIZE - iso14443_3a_data->uid_len - skip_byte;

                memcpy(
                    mf_classic_data->block[0].data + iso14443_3a_data->uid_len + skip_byte,
                    instance->edit_buffer,
                    byte_num);
            } else if(block_view == MfcEditorBlockViewKeyA) {
                memcpy(
                    mf_classic_get_sector_trailer_by_sector(
                        mf_classic_data, instance->current_sector)
                        ->key_a.data,
                    instance->edit_buffer,
                    MF_CLASSIC_KEY_SIZE);
            } else if(block_view == MfcEditorBlockViewKeyB) {
                memcpy(
                    mf_classic_get_sector_trailer_by_sector(
                        mf_classic_data, instance->current_sector)
                        ->key_b.data,
                    instance->edit_buffer,
                    MF_CLASSIC_KEY_SIZE);
            } else if(block_view == MfcEditorBlockViewUserByte) {
                mf_classic_get_sector_trailer_by_sector(mf_classic_data, instance->current_sector)
                    ->access_bits.data[3] = instance->edit_buffer[0];
            }
            instance->is_unsaved_changes = true;
            scene_manager_previous_scene(instance->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void mfc_editor_scene_data_edit_on_exit(void* context) {
    MfcEditorApp* instance = context;

    if(instance->edit_buffer != NULL) {
        free(instance->edit_buffer);
        instance->edit_buffer = NULL;
    }
}
