#include "nfc_dev.h"

#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

static const char* nfc_file_header = "Flipper NFC device";
static const uint32_t nfc_file_version = 3;
static const uint32_t nfc_file_version_with_lsb_atqa = 2;

// static const char* nfc_keys_file_header = "Flipper NFC keys";
// static const uint32_t nfc_keys_file_version = 1;

// // Protocols format versions
// static const uint32_t nfc_mifare_classic_data_format_version = 2;
static const uint32_t nfc_mifare_ultralight_data_format_version = 1;

struct NfcDev {
    bool shadow_file_exist;

    NfcLoadingCallback loading_callback;
    void* loading_callback_context;
};

typedef bool (*NfcDevVerifyProtocol)(FuriString* device_type, NfcDevData* data);
typedef bool (*NfcDevDataHandler)(FlipperFormat* file, uint32_t version, NfcDevData* data);

typedef struct {
    NfcDevVerifyProtocol verify_handler;
    NfcDevDataHandler load_handler;
    NfcDevDataHandler save_handler;
} NfcDevDataParser;

static bool nfc_dev_nfca_load_data(FlipperFormat* file, uint32_t version, NfcaData* data) {
    furi_assert(file);
    furi_assert(data);

    uint32_t data_cnt = 0;
    bool parsed = false;

    do {
        if(!flipper_format_get_value_count(file, "UID", &data_cnt)) break;
        if(!(data_cnt == 4 || data_cnt == 7)) break;
        data->uid_len = data_cnt;
        if(!flipper_format_read_hex(file, "UID", data->uid, data->uid_len)) break;
        if(version == nfc_file_version_with_lsb_atqa) {
            if(!flipper_format_read_hex(file, "ATQA", data->atqa, 2)) break;
        } else {
            uint8_t atqa[2] = {};
            if(!flipper_format_read_hex(file, "ATQA", atqa, 2)) break;
            data->atqa[0] = atqa[1];
            data->atqa[1] = atqa[0];
        }
        if(!flipper_format_read_hex(file, "SAK", &data->sak, 1)) break;

        parsed = true;
    } while(false);

    return parsed;
}

static bool nfc_dev_nfca_save_data(FlipperFormat* file, NfcaData* data) {
    furi_assert(file);
    furi_assert(data);

    bool saved = false;
    do {
        // Write UID, ATQA, SAK
        if(!flipper_format_write_comment_cstr(file, "UID, ATQA and SAK are common for all formats"))
            break;
        if(!flipper_format_write_hex(file, "UID", data->uid, data->uid_len)) break;
        // Save ATQA in MSB order for correct companion apps display
        uint8_t atqa[2] = {data->atqa[1], data->atqa[0]};
        if(!flipper_format_write_hex(file, "ATQA", atqa, 2)) break;
        if(!flipper_format_write_hex(file, "SAK", &data->sak, 1)) break;
        saved = true;
    } while(false);

    return saved;
}

static bool nfc_dev_nfca_verify_handler(FuriString* device_type, NfcDevData* data) {
    furi_assert(device_type);
    furi_assert(data);

    bool verified = (furi_string_cmp_str(device_type, "UID") == 0);
    if(verified) {
        data->protocol = NfcDevProtocolNfca;
    }

    return verified;
}

static bool nfc_dev_nfca_save_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);
    UNUSED(version);

    bool saved = false;
    do {
        if(!flipper_format_write_string_cstr(file, "Device type", "UID")) break;
        if(!nfc_dev_nfca_save_data(file, &data->nfca_data)) break;
        saved = true;
    } while(false);

    return saved;
}

static bool nfc_dev_nfca_load_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);
    UNUSED(version);

    return nfc_dev_nfca_load_data(file, version, &data->nfca_data);
}

static bool nfc_dev_mf_ultralight_verify_handler(FuriString* device_type, NfcDevData* data) {
    furi_assert(device_type);
    furi_assert(data);

    bool verified = false;
    for(size_t i = 0; i < MfUltralightTypeNum; i++) {
        const char* name = mf_ultralight_get_name(i, true);
        verified = furi_string_equal_str(device_type, name);
        if(verified) {
            data->mf_ul_data.type = i;
            break;
        }
    }

    return verified;
}

static bool
    nfc_dev_mf_ultralight_save_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);
    UNUSED(version);

    FuriString* temp_str = furi_string_alloc();
    MfUltralightData* mfu_data = &data->mf_ul_data;
    bool saved = false;
    do {
        if(!nfc_dev_nfca_save_data(file, &data->nfca_data)) break;
        if(!flipper_format_write_comment_cstr(file, "Mifare Ultralight specific data")) break;
        if(!flipper_format_write_uint32(
               file, "Data format version", &nfc_mifare_ultralight_data_format_version, 1))
            break;
        if(!flipper_format_write_hex(
               file, "Signature", mfu_data->signature.data, sizeof(MfUltralightSignature)))
            break;
        if(!flipper_format_write_hex(
               file, "Mifare version", (uint8_t*)&mfu_data->version, sizeof(MfUltralightVersion)))
            break;

        // Write conters and tearing flags data
        bool counters_saved = true;
        for(size_t i = 0; i < 3; i++) {
            furi_string_printf(temp_str, "Counter %d", i);
            if(!flipper_format_write_uint32(
                   file, furi_string_get_cstr(temp_str), &mfu_data->counter[i].counter, 1)) {
                counters_saved = false;
                break;
            }
            furi_string_printf(temp_str, "Tearing %d", i);
            if(!flipper_format_write_hex(
                   file, furi_string_get_cstr(temp_str), mfu_data->tearing_flag->data, 1)) {
                counters_saved = false;
                break;
            }
        }
        if(!counters_saved) break;

        // Write pages data
        if(!flipper_format_write_uint32(file, "Pages total", (uint32_t*)&mfu_data->pages_total, 1))
            break;
        if(!flipper_format_write_uint32(file, "Pages read", (uint32_t*)&mfu_data->pages_read, 1))
            break;
        bool pages_saved = true;
        for(size_t i = 0; i < mfu_data->pages_total; i++) {
            furi_string_printf(temp_str, "Page %d", i);
            if(!flipper_format_write_hex(
                   file,
                   furi_string_get_cstr(temp_str),
                   mfu_data->page[i].data,
                   sizeof(MfUltralightPage))) {
                pages_saved = false;
                break;
            }
        }
        if(!pages_saved) break;

        // Write authentication counter
        if(!flipper_format_write_uint32(
               file, "Failed authentication attempts", &mfu_data->auth_attempts, 1))
            break;

        saved = true;
    } while(false);

    furi_string_free(temp_str);

    return saved;
}

static bool
    nfc_dev_mf_ultralight_load_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);

    FuriString* temp_str = furi_string_alloc();
    bool parsed = false;
    do {
        // Read NFCA data
        if(!nfc_dev_nfca_load_data(file, version, &data->mf_ul_data.nfca_data)) break;

        // Read Ultralight specific data
        // Read Mifare Ultralight format version
        uint32_t data_format_version = 0;
        if(!flipper_format_read_uint32(file, "Data format version", &data_format_version, 1)) {
            if(!flipper_format_rewind(file)) break;
        }

        // Read signature
        MfUltralightData* mfu_data = &data->mf_ul_data;
        if(!flipper_format_read_hex(
               file, "Signature", mfu_data->signature.data, sizeof(MfUltralightSignature)))
            break;
        // Read Mifare version
        if(!flipper_format_read_hex(
               file, "Mifare version", (uint8_t*)&mfu_data->version, sizeof(MfUltralightVersion)))
            break;
        // Read counters and tearing flags
        bool counters_parsed = true;
        for(size_t i = 0; i < 3; i++) {
            furi_string_printf(temp_str, "Counter %d", i);
            if(!flipper_format_read_uint32(
                   file, furi_string_get_cstr(temp_str), &mfu_data->counter[i].counter, 1)) {
                counters_parsed = false;
                break;
            }
            furi_string_printf(temp_str, "Tearing %d", i);
            if(!flipper_format_read_hex(
                   file, furi_string_get_cstr(temp_str), mfu_data->tearing_flag[i].data, 1)) {
                counters_parsed = false;
                break;
            }
        }
        if(!counters_parsed) break;
        // Read pages
        uint32_t pages_total = 0;
        if(!flipper_format_read_uint32(file, "Pages total", &pages_total, 1)) break;
        uint32_t pages_read = 0;
        if(data_format_version < nfc_mifare_ultralight_data_format_version) {
            pages_read = pages_total;
        } else {
            if(!flipper_format_read_uint32(file, "Pages read", &pages_read, 1)) break;
        }
        mfu_data->pages_total = pages_total;
        mfu_data->pages_read = pages_read;

        // data->data_size = pages_total * 4;
        // data->data_read = pages_read * 4;
        if((pages_read > MF_ULTRALIGHT_MAX_PAGE_NUM) || (pages_total > MF_ULTRALIGHT_MAX_PAGE_NUM))
            break;

        bool pages_parsed = true;
        for(size_t i = 0; i < pages_total; i++) {
            furi_string_printf(temp_str, "Page %d", i);
            if(!flipper_format_read_hex(
                   file,
                   furi_string_get_cstr(temp_str),
                   mfu_data->page[i].data,
                   sizeof(MfUltralightPage))) {
                pages_parsed = false;
                break;
            }
        }
        if(!pages_parsed) break;

        // Read authentication counter
        if(!flipper_format_read_uint32(
               file, "Failed authentication attempts", &mfu_data->auth_attempts, 1)) {
            mfu_data->auth_attempts = 0;
        }

        parsed = true;
    } while(false);

    furi_string_free(temp_str);

    return parsed;
}

static bool nfc_dev_mf_classic_verify_handler(FuriString* device_type, NfcDevData* data) {
    furi_assert(device_type);
    furi_assert(data);

    return furi_string_start_with_str(device_type, "Mifare Classic");
}

static bool
    nfc_dev_mf_classic_save_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);
    UNUSED(version);

    bool saved = false;

    do {
        if(!nfc_dev_nfca_save_data(file, &data->nfca_data)) break;
        saved = true;
    } while(false);

    return saved;
}

static bool
    nfc_dev_mf_classic_load_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);

    bool parsed = false;
    do {
        // Read NFCA data
        if(!nfc_dev_nfca_load_data(file, version, &data->mf_ul_data.nfca_data)) break;

        parsed = true;
    } while(false);

    return parsed;
}

static bool nfc_dev_mf_desfire_verify_handler(FuriString* device_type, NfcDevData* data) {
    furi_assert(device_type);
    furi_assert(data);

    return furi_string_start_with_str(device_type, "Mifare Desfire");
}

static bool
    nfc_dev_mf_desfire_save_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);
    UNUSED(version);

    bool saved = false;

    do {
        if(!nfc_dev_nfca_save_data(file, &data->nfca_data)) break;
        saved = true;
    } while(false);

    return saved;
}

static bool
    nfc_dev_mf_desfire_load_handler(FlipperFormat* file, uint32_t version, NfcDevData* data) {
    furi_assert(file);
    furi_assert(data);

    bool parsed = false;
    do {
        // Read NFCA data
        if(!nfc_dev_nfca_load_data(file, version, &data->mf_ul_data.nfca_data)) break;

        parsed = true;
    } while(false);

    return parsed;
}

static const NfcDevDataParser nfc_dev_data_parser[NfcDevProtocolNum] = {
    [NfcDevProtocolNfca] =
        {
            .verify_handler = nfc_dev_nfca_verify_handler,
            .save_handler = nfc_dev_nfca_save_handler,
            .load_handler = nfc_dev_nfca_load_handler,
        },
    [NfcDevProtocolMfUltralight] =
        {
            .verify_handler = nfc_dev_mf_ultralight_verify_handler,
            .save_handler = nfc_dev_mf_ultralight_save_handler,
            .load_handler = nfc_dev_mf_ultralight_load_handler,
        },
    [NfcDevProtocolMfClassic] =
        {
            .verify_handler = nfc_dev_mf_classic_verify_handler,
            .save_handler = nfc_dev_mf_classic_save_handler,
            .load_handler = nfc_dev_mf_classic_load_handler,
        },
    [NfcDevProtocolMfDesfire] =
        {
            .verify_handler = nfc_dev_mf_desfire_verify_handler,
            .save_handler = nfc_dev_mf_desfire_save_handler,
            .load_handler = nfc_dev_mf_desfire_load_handler,
        },

};

NfcDev* nfc_dev_alloc() {
    NfcDev* instance = malloc(sizeof(NfcDev));

    return instance;
}

void nfc_dev_free(NfcDev* instance) {
    furi_assert(instance);
    free(instance);
}

void nfc_dev_set_loading_callback(NfcDev* instance, NfcLoadingCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->loading_callback = callback;
    instance->loading_callback_context = context;
}

bool nfc_dev_save(NfcDev* instance, NfcDevData* data, const char* path) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(path);

    bool saved = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    FuriString* temp_str;
    temp_str = furi_string_alloc();

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, true);
    }

    do {
        // Open file
        if(!flipper_format_file_open_always(file, path)) break;

        // Write header
        if(!flipper_format_write_header_cstr(file, nfc_file_header, nfc_file_version)) break;

        // Write nfc device type
        if(!flipper_format_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Mifare Classic"))
            break;

        for(size_t i = 0; i < COUNT_OF(nfc_dev_data_parser); i++) {
            if(data->protocol == i) {
                saved = nfc_dev_data_parser[i].save_handler(file, nfc_file_version, data);
            }
            if(saved) break;
        }
    } while(false);

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, false);
    }

    furi_string_free(temp_str);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return saved;
}

bool nfc_dev_load(NfcDev* instance, NfcDevData* data, const char* path) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(path);

    bool loaded = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    FuriString* temp_str;
    temp_str = furi_string_alloc();

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, true);
    }

    do {
        if(!flipper_format_file_open_existing(file, path)) break;

        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, nfc_file_header)) break;
        if(version < nfc_file_version_with_lsb_atqa) break;

        // Read Nfc device type
        if(!flipper_format_read_string(file, "Device type", temp_str)) break;

        for(size_t i = 0; i < COUNT_OF(nfc_dev_data_parser); i++) {
            if(nfc_dev_data_parser[i].verify_handler(temp_str, data)) {
                loaded = nfc_dev_data_parser[i].load_handler(file, version, data);
            }
            if(loaded) break;
        }
    } while(false);

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, false);
    }

    furi_string_free(temp_str);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return loaded;
}
