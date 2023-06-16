#include "mf_desfire_i.h"

void mf_desfire_version_parse(MfDesfireVersion* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireVersion));
}

void mf_desfire_free_memory_parse(MfDesfireFreeMemory* data, const BitBuffer* buf) {
    furi_assert(!data->is_present);

    bit_buffer_write_bytes(buf, &data->bytes_free, sizeof(data->bytes_free) - 1);
    data->bytes_free &= 0x00ffffff;
    data->is_present = true;
}

void mf_desfire_key_settings_parse(MfDesfireKeySettings* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireKeySettings));
}

void mf_desfire_key_version_parse(MfDesfireKeyVersion* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireKeyVersion));
}

void mf_desfire_application_id_parse(
    MfDesfireApplicationId data,
    uint32_t index,
    const BitBuffer* buf) {
    bit_buffer_write_bytes_mid(
        buf, data, index * sizeof(MfDesfireApplicationId), sizeof(MfDesfireApplicationId));
}

void mf_desfire_file_id_parse(MfDesfireFileId* data, uint32_t index, const BitBuffer* buf) {
    bit_buffer_write_bytes_mid(
        buf, data, index * sizeof(MfDesfireFileId), sizeof(MfDesfireFileId));
}

void mf_desfire_application_init(MfDesfireApplication* data) {
    data->key_versions = simple_array_alloc(&mf_desfire_key_version_array_config);
    data->file_ids = simple_array_alloc(&mf_desfire_file_id_array_config);
    data->file_settings = simple_array_alloc(&mf_desfire_file_settings_array_config);
    data->file_data = simple_array_alloc(&mf_desfire_file_data_array_config);
}

// void mf_desfire_file_reset(MfDesfireFile* file) {
//     if(file->contents) {
//         free(file->contents);
//     }
//     memset(file, 0, sizeof(MfDesfireFile));
// }

void mf_desfire_application_reset(MfDesfireApplication* app) {
    simple_array_free(app->key_versions);
    simple_array_free(app->file_ids);
    simple_array_free(app->file_settings);
    simple_array_free(app->file_data);
    memset(app, 0, sizeof(MfDesfireApplication));
}

// void mf_desfire_file_copy(MfDesfireFile* file, const MfDesfireFile* other) {
//     furi_assert(file->contents == NULL);
//
//     file->id = other->id;
//     file->type = other->type;
//     file->comm = other->comm;
//     file->access_rights = other->access_rights;
//
//     if(other->type == MfDesfireFileTypeStandard || other->type == MfDesfireFileTypeBackup) {
//         file->data = other->data;
//         if(other->data.size == 0) {
//             return;
//         }
//
//         file->contents = malloc(other->data.size);
//         memcpy(file->contents, other->contents, other->data.size);
//
//     } else if(other->type == MfDesfireFileTypeValue) {
//         file->value = other->value;
//     } else if(
//         other->type == MfDesfireFileTypeLinearRecord ||
//         other->type == MfDesfireFileTypeCyclicRecord) {
//         file->record = other->record;
//     } else {
//         furi_crash("Invalid file type");
//     }
// }

void mf_desfire_application_copy(MfDesfireApplication* data, const MfDesfireApplication* other) {
    data->key_settings = other->key_settings;
    simple_array_copy(data->key_versions, other->key_versions);
    simple_array_copy(data->file_ids, other->file_ids);
    simple_array_copy(data->file_settings, other->file_settings);
    simple_array_copy(data->file_data, other->file_data);
}

const SimpleArrayConfig mf_desfire_key_version_array_config = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireKeyVersion),
};

const SimpleArrayConfig mf_desfire_app_id_array_config = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireApplicationId),
};

const SimpleArrayConfig mf_desfire_file_id_array_config = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireFileId),
};

const SimpleArrayConfig mf_desfire_file_settings_array_config = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireFileSettings),
};

const SimpleArrayConfig mf_desfire_file_data_array_config = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(uint8_t),
};

const SimpleArrayConfig mf_desfire_application_array_config = {
    .init = (SimpleArrayInit)mf_desfire_application_init,
    .copy = (SimpleArrayCopy)mf_desfire_application_copy,
    .reset = (SimpleArrayReset)mf_desfire_application_reset,
    .type_size = sizeof(MfDesfireApplication),
};
