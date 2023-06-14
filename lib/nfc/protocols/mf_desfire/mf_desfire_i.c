#include "mf_desfire_i.h"

void mf_desfire_version_parse(MfDesfireVersion* version, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, version, sizeof(MfDesfireVersion));
}

void mf_desfire_free_memory_parse(MfDesfireFreeMemory* free_mem, const BitBuffer* buf) {
    furi_assert(!free_mem->is_present);

    bit_buffer_write_bytes(buf, &free_mem->bytes_free, sizeof(free_mem->bytes_free) - 1);
    free_mem->bytes_free &= 0x00ffffff;
    free_mem->is_present = true;
}

void mf_desfire_key_settings_parse(MfDesfireKeySettings* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireKeySettings));
}

void mf_desfire_key_version_parse(MfDesfireKeyVersion* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireKeyVersion));
}

void mf_desfire_application_id_parse(MfDesfireApplicationId data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireApplicationId));
}

// void mf_desfire_key_config_reset(MfDesfireKeyConfiguration* data) {
//     simple_array_reset(data->key_versions);
// }

void mf_desfire_file_reset(MfDesfireFile* file) {
    if(file->contents) {
        free(file->contents);
    }
    memset(file, 0, sizeof(MfDesfireFile));
}

void mf_desfire_files_reset(MfDesfireFiles* files) {
    if(files->data) {
        for(size_t i = 0; i < files->count; ++i) {
            mf_desfire_file_reset(&files->data[i]);
        }
        free(files->data);
    }
    memset(files, 0, sizeof(MfDesfireFiles));
}

// void mf_desfire_application_reset(MfDesfireApplication* app) {
//     memset(app->id, 0, sizeof(app->id));
//     mf_desfire_key_config_reset(&app->key_config);
//     mf_desfire_files_reset(&app->files);
// }

// void mf_desfire_applications_reset(MfDesfireApplications* apps) {
//     if(apps->data) {
//         for(size_t i = 0; i < apps->count; ++i) {
//             mf_desfire_application_reset(&apps->data[i]);
//         }
//         free(apps->data);
//     }
//     memset(apps, 0, sizeof(MfDesfireApplications));
// }

// void mf_desfire_key_config_copy(
//     MfDesfireKeyConfiguration* config,
//     const MfDesfireKeyConfiguration* other) {
//     // furi_assert(config->key_versions == NULL);
//
//     config->key_settings = other->key_settings;
//     simple_array_copy(config->key_versions, other->key_versions);
// }

void mf_desfire_file_copy(MfDesfireFile* file, const MfDesfireFile* other) {
    furi_assert(file->contents == NULL);

    file->id = other->id;
    file->type = other->type;
    file->comm = other->comm;
    file->access_rights = other->access_rights;

    if(other->type == MfDesfireFileTypeStandard || other->type == MfDesfireFileTypeBackup) {
        file->data = other->data;
        if(other->data.size == 0) {
            return;
        }

        file->contents = malloc(other->data.size);
        memcpy(file->contents, other->contents, other->data.size);

    } else if(other->type == MfDesfireFileTypeValue) {
        file->value = other->value;
    } else if(
        other->type == MfDesfireFileTypeLinearRecord ||
        other->type == MfDesfireFileTypeCyclicRecord) {
        file->record = other->record;
    } else {
        furi_crash("Invalid file type");
    }
}

void mf_desfire_files_copy(MfDesfireFiles* files, const MfDesfireFiles* other) {
    furi_assert(files->data == NULL);

    files->count = other->count;
    if(other->count == 0) {
        return;
    }

    files->data = malloc(other->count * sizeof(MfDesfireFile));

    for(size_t i = 0; i < other->count; ++i) {
        mf_desfire_file_copy(&files->data[i], &other->data[i]);
    }
}

// void mf_desfire_application_copy(MfDesfireApplication* app, const MfDesfireApplication* other) {
//     memcpy(app->id, other->id, sizeof(other->id));
//     mf_desfire_key_config_copy(&app->key_config, &other->key_config);
//     mf_desfire_files_copy(&app->files, &other->files);
// }
//
// void mf_desfire_applications_copy(MfDesfireApplications* apps, const MfDesfireApplications* other) {
//     furi_assert(apps->data == NULL);
//
//     apps->count = other->count;
//     if(other->count == 0) {
//         return;
//     }
//
//     apps->data = malloc(other->count * sizeof(MfDesfireApplication));
//
//     for(size_t i = 0; i < other->count; ++i) {
//         mf_desfire_application_copy(&apps->data[i], &other->data[i]);
//     }
// }

const SimpleArrayConfig mf_desfire_key_version_array_config = {
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireKeyVersion),
};

const SimpleArrayConfig mf_desfire_app_id_array_config = {
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireApplicationId),
};

// TODO: Custom methods
const SimpleArrayConfig mf_desfire_app_array_config = {
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(MfDesfireApplication),
};
