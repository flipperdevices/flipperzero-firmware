#include "mf_desfire.h"

#include <furi.h>

#define MF_DESFIRE_PROTOCOL_NAME "Mifare DESfire"

const NfcProtocolBase nfc_protocol_mf_desfire = {
    .alloc = (NfcProtocolAlloc)mf_desfire_alloc,
    .free = (NfcProtocolFree)mf_desfire_free,
    .reset = (NfcProtocolReset)mf_desfire_reset,
    .copy = (NfcProtocolCopy)mf_desfire_copy,
    .verify = (NfcProtocolVerify)mf_desfire_verify,
    .load = (NfcProtocolLoad)mf_desfire_load,
    .save = (NfcProtocolSave)mf_desfire_save,
    .is_equal = (NfcProtocolEqual)mf_desfire_is_equal,
    .get_name = (NfcProtocolGetName)mf_desfire_get_name,
    .get_uid = (NfcProtocolGetUid)mf_desfire_get_uid,
};

MfDesfireData* mf_desfire_alloc() {
    MfDesfireData* data = malloc(sizeof(MfDesfireData));
    data->iso14443_4a_data = iso14443_4a_alloc();
    return data;
}

void mf_desfire_free(MfDesfireData* data) {
    furi_assert(data);
    mf_desfire_reset(data);
    iso14443_4a_free(data->iso14443_4a_data);
    free(data);
}

static inline void mf_desfire_reset_key_config(MfDesfireKeyConfiguration* config) {
    memset(&config->key_settings, 0, sizeof(MfDesfireKeySettings));
    if(config->key_versions) {
        free(config->key_versions);
        config->key_versions = NULL;
    }
}

static inline void mf_desfire_reset_file(MfDesfireFile* file) {
    if(file->type == MfDesfireFileTypeStandard || file->type == MfDesfireFileTypeBackup) {
        memset(&file->data, 0, sizeof(file->data));
        if(file->contents) {
            free(file->contents);
            file->contents = NULL;
        }

    } else if(file->type == MfDesfireFileTypeValue) {
        memset(&file->value, 0, sizeof(file->value));
    } else if(
        file->type == MfDesfireFileTypeLinearRecord ||
        file->type == MfDesfireFileTypeCyclicRecord) {
        memset(&file->record, 0, sizeof(file->record));
    } else {
        furi_crash("Invalid file type");
    }

    file->id = 0;
    file->type = 0;
    file->comm = 0;
    file->type = 0;
}

static inline void mf_desfire_reset_files(MfDesfireFiles* files) {
    for(size_t i = 0; i < files->count; ++i) {
        mf_desfire_reset_file(&files->data[i]);
    }

    files->count = 0;

    if(files->data) {
        free(files->data);
        files->data = NULL;
    }
}

static inline void mf_desfire_reset_application(MfDesfireApplication* app) {
    memset(app->id, 0, sizeof(app->id));
    mf_desfire_reset_key_config(&app->key_config);
    mf_desfire_reset_files(&app->files);
}

static inline void mf_desfire_reset_applications(MfDesfireApplications* apps) {
    for(size_t i = 0; i < apps->count; ++i) {
        mf_desfire_reset_application(&apps->data[i]);
    }

    apps->count = 0;

    if(apps->data) {
        free(apps->data);
        apps->data = NULL;
    }
}

void mf_desfire_reset(MfDesfireData* data) {
    furi_assert(data);

    iso14443_4a_reset(data->iso14443_4a_data);

    memset(&data->version, 0, sizeof(MfDesfireVersion));
    memset(&data->free_memory, 0, sizeof(MfDesfireFreeMemory));

    mf_desfire_reset_key_config(&data->master_key);
    mf_desfire_reset_applications(&data->applications);
}

static inline void mf_desfire_copy_key_config(
    MfDesfireKeyConfiguration* config,
    const MfDesfireKeyConfiguration* other) {
    furi_assert(config->key_versions == NULL);

    config->key_settings = other->key_settings;
    if(other->key_settings.max_keys == 0) {
        return;
    }

    const size_t key_versions_size = other->key_settings.max_keys * sizeof(MfDesfireKeyVersion);
    config->key_versions = malloc(key_versions_size);
    memcpy(config->key_versions, other->key_versions, key_versions_size);
}

static inline void mf_desfire_copy_file(MfDesfireFile* file, const MfDesfireFile* other) {
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

static inline void mf_desfire_copy_files(MfDesfireFiles* files, const MfDesfireFiles* other) {
    furi_assert(files->data == NULL);

    files->count = other->count;
    if(other->count == 0) {
        return;
    }

    files->data = malloc(other->count * sizeof(MfDesfireFile));

    for(size_t i = 0; i < other->count; ++i) {
        mf_desfire_copy_file(&files->data[i], &other->data[i]);
    }
}

static inline void
    mf_desfire_copy_application(MfDesfireApplication* app, const MfDesfireApplication* other) {
    memcpy(app->id, other->id, sizeof(other->id));
    mf_desfire_copy_key_config(&app->key_config, &other->key_config);
    mf_desfire_copy_files(&app->files, &other->files);
}

static inline void
    mf_desfire_copy_applications(MfDesfireApplications* apps, const MfDesfireApplications* other) {
    furi_assert(apps->data == NULL);

    apps->count = other->count;
    if(other->count == 0) {
        return;
    }

    apps->data = malloc(other->count * sizeof(MfDesfireApplication));

    for(size_t i = 0; i < other->count; ++i) {
        mf_desfire_copy_application(&apps->data[i], &other->data[i]);
    }
}

void mf_desfire_copy(MfDesfireData* data, const MfDesfireData* other) {
    furi_assert(data);
    furi_assert(other);

    mf_desfire_reset(data);

    iso14443_4a_copy(data->iso14443_4a_data, other->iso14443_4a_data);

    data->version = other->version;
    data->free_memory = other->free_memory;

    mf_desfire_copy_key_config(&data->master_key, &other->master_key);
    mf_desfire_copy_applications(&data->applications, &other->applications);
}

bool mf_desfire_verify(MfDesfireData* data, const FuriString* device_type) {
    UNUSED(data);
    return furi_string_equal_str(device_type, "Mifare Desfire");
}

bool mf_desfire_load(MfDesfireData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: Implementation
    return false;
}

bool mf_desfire_save(const MfDesfireData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: Implementation
    return false;
}

bool mf_desfire_is_equal(const MfDesfireData* data, const MfDesfireData* other) {
    furi_assert(data);
    furi_assert(other);

    // TODO: Complete equality method
    return iso14443_4a_is_equal(data->iso14443_4a_data, other->iso14443_4a_data);
}

const char* mf_desfire_get_name(const MfDesfireData* data, NfcProtocolNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);
    return MF_DESFIRE_PROTOCOL_NAME;
}

const uint8_t* mf_desfire_get_uid(const MfDesfireData* data, size_t* uid_len) {
    furi_assert(data);

    return iso14443_4a_get_uid(data->iso14443_4a_data, uid_len);
}

bool mf_desfire_detect_protocol(NfcaData* nfca_data) {
    furi_assert(nfca_data);

    bool mfu_detected = false;
    return mfu_detected;
}
