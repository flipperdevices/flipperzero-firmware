#include "mf_desfire_i.h"

#define MF_DESFIRE_FFF_VERSION_KEY \
    MF_DESFIRE_FFF_PICC_PREFIX " " \
                               "Version"
#define MF_DESFIRE_FFF_FREE_MEM_KEY \
    MF_DESFIRE_FFF_PICC_PREFIX " "  \
                               "Free Memory"

#define MF_DESFIRE_FFF_CHANGE_KEY_ID_KEY "Change Key ID"
#define MF_DESFIRE_FFF_CONFIG_CHANGEABLE_KEY "Config Changeable"
#define MF_DESFIRE_FFF_FREE_CREATE_DELETE_KEY "Free Create Delete"
#define MF_DESFIRE_FFF_FREE_DIR_LIST_KEY "Free Directory List"
#define MF_DESFIRE_FFF_KEY_CHANGEABLE_KEY "Key Changeable"
#define MF_DESFIRE_FFF_FLAGS_KEY "Flags"
#define MF_DESFIRE_FFF_MAX_KEYS_KEY "Max Keys"

#define MF_DESFIRE_FFF_KEY_SUB_PREFIX "Key"
#define MF_DESFIRE_FFF_KEY_VERSION_KEY "Version"

#define MF_DESFIRE_FFF_APPLICATION_COUNT_KEY \
    MF_DESFIRE_FFF_APP_PREFIX " "            \
                              "Count"
#define MF_DESFIRE_FFF_APPLICATION_IDS_KEY \
    MF_DESFIRE_FFF_APP_PREFIX " "          \
                              "IDs"

#define MF_DESFIRE_FFF_FILE_SUB_PREFIX "File"
#define MF_DESFIRE_FFF_FILE_IDS_KEY    \
    MF_DESFIRE_FFF_FILE_SUB_PREFIX " " \
                                   "IDs"
#define MF_DESFIRE_FFF_FILE_TYPE_KEY "Type"
#define MF_DESFIRE_FFF_FILE_COMM_SETTINGS_KEY "Communication Settings"
#define MF_DESFIRE_FFF_FILE_ACCESS_RIGHTS_KEY "Access Rights"

#define MF_DESFIRE_FFF_FILE_SIZE_KEY "Size"

#define MF_DESFIRE_FFF_FILE_HI_LIMIT_KEY "Hi Limit"
#define MF_DESFIRE_FFF_FILE_LO_LIMIT_KEY "Lo Limit"
#define MF_DESFIRE_FFF_FILE_LIMIT_CREDIT_VALUE_KEY "Limited Credit Value"
#define MF_DESFIRE_FFF_FILE_LIMIT_CREDIT_ENABLED_KEY "Limited Credit Enabled"

#define MF_DESFIRE_FFF_FILE_MAX_KEY "Max"
#define MF_DESFIRE_FFF_FILE_CUR_KEY "Cur"

typedef struct {
    bool is_master_key_changeable : 1;
    bool is_free_directory_list : 1;
    bool is_free_create_delete : 1;
    bool is_config_changeable : 1;
    uint8_t change_key_id : 4;
    uint8_t max_keys : 4;
    uint8_t flags : 4;
} MfDesfireKeySettingsLayout;

void mf_desfire_version_parse(MfDesfireVersion* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireVersion));
}

void mf_desfire_free_memory_parse(MfDesfireFreeMemory* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, &data->bytes_free, sizeof(data->bytes_free) - 1);
    data->bytes_free &= 0x00ffffff;
    data->is_present = true;
}

void mf_desfire_key_settings_parse(MfDesfireKeySettings* data, const BitBuffer* buf) {
    MfDesfireKeySettingsLayout layout;
    bit_buffer_write_bytes(buf, &layout, sizeof(MfDesfireKeySettingsLayout));

    data->is_master_key_changeable = layout.is_master_key_changeable;
    data->is_free_directory_list = layout.is_free_directory_list;
    data->is_free_create_delete = layout.is_free_create_delete;
    data->is_config_changeable = layout.is_config_changeable;

    data->change_key_id = layout.change_key_id;
    data->max_keys = layout.max_keys;
    data->flags = layout.flags;
}

void mf_desfire_key_version_parse(MfDesfireKeyVersion* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireKeyVersion));
}

void mf_desfire_application_id_parse(
    MfDesfireApplicationId* data,
    uint32_t index,
    const BitBuffer* buf) {
    bit_buffer_write_bytes_mid(
        buf, data, index * sizeof(MfDesfireApplicationId), sizeof(MfDesfireApplicationId));
}

void mf_desfire_file_id_parse(MfDesfireFileId* data, uint32_t index, const BitBuffer* buf) {
    bit_buffer_write_bytes_mid(
        buf, data, index * sizeof(MfDesfireFileId), sizeof(MfDesfireFileId));
}

void mf_desfire_file_settings_parse(MfDesfireFileSettings* data, const BitBuffer* buf) {
    bit_buffer_write_bytes(buf, data, sizeof(MfDesfireFileSettings));
}

void mf_desfire_file_data_parse(MfDesfireFileData* data, const BitBuffer* buf) {
    const size_t data_size = bit_buffer_get_size_bytes(buf);

    if(data_size > 0) {
        simple_array_init(data->data, data_size);
        bit_buffer_write_bytes(buf, simple_array_get(data->data, 0), data_size);
    }
}

void mf_desfire_file_data_init(MfDesfireFileData* data) {
    data->data = simple_array_alloc(&mf_desfire_file_data_element_array_config);
}

void mf_desfire_application_init(MfDesfireApplication* data) {
    data->key_versions = simple_array_alloc(&mf_desfire_key_version_array_config);
    data->file_ids = simple_array_alloc(&mf_desfire_file_id_array_config);
    data->file_settings = simple_array_alloc(&mf_desfire_file_settings_array_config);
    data->file_data = simple_array_alloc(&mf_desfire_file_data_array_config);
}

void mf_desfire_file_data_reset(MfDesfireFileData* data) {
    simple_array_free(data->data);
    memset(data, 0, sizeof(MfDesfireFileData));
}

void mf_desfire_application_reset(MfDesfireApplication* data) {
    simple_array_free(data->key_versions);
    simple_array_free(data->file_ids);
    simple_array_free(data->file_settings);
    simple_array_free(data->file_data);
    memset(data, 0, sizeof(MfDesfireApplication));
}

void mf_desfire_file_data_copy(MfDesfireFileData* data, const MfDesfireFileData* other) {
    simple_array_copy(data->data, other->data);
}

void mf_desfire_application_copy(MfDesfireApplication* data, const MfDesfireApplication* other) {
    data->key_settings = other->key_settings;
    simple_array_copy(data->key_versions, other->key_versions);
    simple_array_copy(data->file_ids, other->file_ids);
    simple_array_copy(data->file_settings, other->file_settings);
    simple_array_copy(data->file_data, other->file_data);
}

bool mf_desfire_version_load(MfDesfireVersion* data, FlipperFormat* ff) {
    return flipper_format_read_hex(
        ff, MF_DESFIRE_FFF_VERSION_KEY, (uint8_t*)data, sizeof(MfDesfireVersion));
}

bool mf_desfire_free_memory_load(MfDesfireFreeMemory* data, FlipperFormat* ff) {
    data->is_present = flipper_format_key_exist(ff, MF_DESFIRE_FFF_FREE_MEM_KEY);
    return data->is_present ?
               flipper_format_read_uint32(ff, MF_DESFIRE_FFF_FREE_MEM_KEY, &data->bytes_free, 1) :
               true;
}

bool mf_desfire_key_settings_load(
    MfDesfireKeySettings* data,
    const char* prefix,
    FlipperFormat* ff) {
    bool is_loaded = false;

    FuriString* key = furi_string_alloc();

    do {
        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_CHANGE_KEY_ID_KEY);
        if(!flipper_format_read_hex(ff, furi_string_get_cstr(key), &data->change_key_id, 1)) break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_CONFIG_CHANGEABLE_KEY);
        if(!flipper_format_read_bool(ff, furi_string_get_cstr(key), &data->is_config_changeable, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FREE_CREATE_DELETE_KEY);
        if(!flipper_format_read_bool(
               ff, furi_string_get_cstr(key), &data->is_free_create_delete, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FREE_DIR_LIST_KEY);
        if(!flipper_format_read_bool(
               ff, furi_string_get_cstr(key), &data->is_free_directory_list, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_KEY_CHANGEABLE_KEY);
        if(!flipper_format_read_bool(
               ff, furi_string_get_cstr(key), &data->is_master_key_changeable, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FLAGS_KEY);
        if(flipper_format_key_exist(ff, furi_string_get_cstr(key))) {
            if(!flipper_format_read_hex(ff, furi_string_get_cstr(key), &data->flags, 1)) break;
        }

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_MAX_KEYS_KEY);
        if(!flipper_format_read_hex(ff, furi_string_get_cstr(key), &data->max_keys, 1)) break;

        // TODO: Whaaa
        // ks->flags |= ks->max_keys >> 4;
        // ks->max_keys &= 0xF;

        is_loaded = true;
    } while(false);

    furi_string_free(key);
    return is_loaded;
}

bool mf_desfire_key_version_load(
    MfDesfireKeyVersion* data,
    const char* prefix,
    uint32_t index,
    FlipperFormat* ff) {
    FuriString* key = furi_string_alloc_printf(
        "%s %s %lu %s",
        prefix,
        MF_DESFIRE_FFF_KEY_SUB_PREFIX,
        index,
        MF_DESFIRE_FFF_KEY_VERSION_KEY);
    const bool is_loaded = flipper_format_read_hex(ff, furi_string_get_cstr(key), data, 1);
    furi_string_free(key);
    return is_loaded;
}

bool mf_desfire_file_count_load(uint32_t* data, const char* prefix, FlipperFormat* ff) {
    FuriString* key = furi_string_alloc_printf("%s %s", prefix, MF_DESFIRE_FFF_FILE_IDS_KEY);
    const bool is_loaded = flipper_format_get_value_count(ff, furi_string_get_cstr(key), data);
    furi_string_free(key);
    return is_loaded;
}

bool mf_desfire_file_ids_load(
    MfDesfireFileId* data,
    uint32_t count,
    const char* prefix,
    FlipperFormat* ff) {
    FuriString* key = furi_string_alloc_printf("%s %s", prefix, MF_DESFIRE_FFF_FILE_IDS_KEY);
    const bool is_loaded = flipper_format_read_hex(ff, furi_string_get_cstr(key), data, count);
    furi_string_free(key);
    return is_loaded;
}

bool mf_desfire_file_settings_load(
    MfDesfireFileSettings* data,
    const char* prefix,
    FlipperFormat* ff) {
    bool is_loaded = false;

    FuriString* key = furi_string_alloc();

    do {
        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_TYPE_KEY);
        if(!flipper_format_read_hex(ff, furi_string_get_cstr(key), (uint8_t*)&data->type, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_COMM_SETTINGS_KEY);
        if(!flipper_format_read_hex(ff, furi_string_get_cstr(key), (uint8_t*)&data->comm, 1))
            break;

        furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_ACCESS_RIGHTS_KEY);
        if(!flipper_format_read_hex(
               ff,
               furi_string_get_cstr(key),
               (uint8_t*)&data->access_rights,
               sizeof(MfDesfireFileAccessRights)))
            break;

        if(data->type == MfDesfireFileTypeStandard || data->type == MfDesfireFileTypeBackup) {
            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_SIZE_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->data.size, 1))
                break;

        } else if(data->type == MfDesfireFileTypeValue) {
            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_HI_LIMIT_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->value.hi_limit, 1))
                break;

            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_LO_LIMIT_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->value.lo_limit, 1))
                break;

            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_LIMIT_CREDIT_VALUE_KEY);
            if(!flipper_format_read_uint32(
                   ff, furi_string_get_cstr(key), &data->value.limited_credit_value, 1))
                break;

            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_LIMIT_CREDIT_ENABLED_KEY);
            if(!flipper_format_read_bool(
                   ff, furi_string_get_cstr(key), &data->value.limited_credit_enabled, 1))
                break;
        } else if(
            data->type == MfDesfireFileTypeLinearRecord ||
            data->type == MfDesfireFileTypeCyclicRecord) {
            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_SIZE_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->record.size, 1))
                break;

            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_MAX_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->record.max, 1))
                break;

            furi_string_printf(key, "%s %s", prefix, MF_DESFIRE_FFF_FILE_CUR_KEY);
            if(!flipper_format_read_uint32(ff, furi_string_get_cstr(key), &data->record.cur, 1))
                break;
        }

        is_loaded = true;
    } while(false);

    furi_string_free(key);

    return is_loaded;
}

bool mf_desfire_file_data_load(MfDesfireFileData* data, const char* prefix, FlipperFormat* ff) {
    bool is_loaded = false;
    do {
        if(!flipper_format_key_exist(ff, prefix)) {
            is_loaded = true;
            break;
        }

        uint32_t data_size;
        if(!flipper_format_get_value_count(ff, prefix, &data_size)) break;

        simple_array_init(data->data, data_size);

        if(!flipper_format_read_hex(ff, prefix, simple_array_get(data->data, 0), data_size)) break;

        is_loaded = true;
    } while(false);

    return is_loaded;
}

bool mf_desfire_application_count_load(uint32_t* data, FlipperFormat* ff) {
    return flipper_format_read_uint32(ff, MF_DESFIRE_FFF_APPLICATION_COUNT_KEY, data, 1);
}

bool mf_desfire_application_ids_load(
    MfDesfireApplicationId* data,
    uint32_t count,
    FlipperFormat* ff) {
    return flipper_format_read_hex(
        ff,
        MF_DESFIRE_FFF_APPLICATION_IDS_KEY,
        (uint8_t*)data,
        count * sizeof(MfDesfireApplicationId));
}

bool mf_desfire_application_load(MfDesfireApplication* data, const char* prefix, FlipperFormat* ff) {
    FuriString* sub_prefix = furi_string_alloc();
    bool is_loaded = false;

    do {
        // Load key settings
        if(!mf_desfire_key_settings_load(&data->key_settings, prefix, ff)) break;
        // Load key versions
        const uint32_t key_version_count = data->key_settings.max_keys;
        simple_array_init(data->key_versions, key_version_count);

        uint32_t i;
        for(i = 0; i < key_version_count; ++i) {
            if(!mf_desfire_key_version_load(simple_array_get(data->key_versions, i), prefix, i, ff))
                break;
        }

        if(i != key_version_count) break;

        // Load file id count
        uint32_t file_count;
        if(!mf_desfire_file_count_load(&file_count, prefix, ff)) break;

        // Load file ids
        simple_array_init(data->file_ids, file_count);
        if(!mf_desfire_file_ids_load(simple_array_get(data->file_ids, 0), file_count, prefix, ff))
            break;

        simple_array_init(data->file_settings, file_count);
        simple_array_init(data->file_data, file_count);

        // For all files
        for(i = 0; i < file_count; ++i) {
            const MfDesfireFileId* file_id = simple_array_cget(data->file_ids, i);
            furi_string_printf(
                sub_prefix, "%s %s %u", prefix, MF_DESFIRE_FFF_FILE_SUB_PREFIX, *file_id);

            // Load file settings
            MfDesfireFileSettings* file_settings = simple_array_get(data->file_settings, i);
            if(!mf_desfire_file_settings_load(file_settings, furi_string_get_cstr(sub_prefix), ff))
                break;

            // Load file data
            MfDesfireFileData* file_data = simple_array_get(data->file_data, i);
            if(!mf_desfire_file_data_load(file_data, furi_string_get_cstr(sub_prefix), ff)) break;
        }

        if(i != file_count) break;

        is_loaded = true;
    } while(false);

    furi_string_free(sub_prefix);
    return is_loaded;
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
    .init = (SimpleArrayInit)mf_desfire_file_data_init,
    .copy = (SimpleArrayCopy)mf_desfire_file_data_copy,
    .reset = (SimpleArrayReset)mf_desfire_file_data_reset,
    .type_size = sizeof(MfDesfireData),
};

const SimpleArrayConfig mf_desfire_file_data_element_array_config = {
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
