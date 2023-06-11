#include "nfc_dev.h"

#include <nfc/nfc_common.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#define NFC_FILE_HEADER "Flipper NFC device"
#define NFC_DEV_TYPE_ERROR "Protocol type mismatch"

struct NfcDev {
    NfcProtocolType protocol_type;
    NfcProtocolData* protocol_data;

    NfcLoadingCallback loading_callback;
    void* loading_callback_context;
};

NfcDev* nfc_dev_alloc() {
    NfcDev* instance = malloc(sizeof(NfcDev));

    return instance;
}

void nfc_dev_free(NfcDev* instance) {
    furi_assert(instance);

    nfc_dev_clear(instance);
    free(instance);
}

void nfc_dev_clear(NfcDev* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol_type < NfcProtocolTypeMax);

    if(instance->protocol_data) {
        nfc_protocols[instance->protocol_type]->free(instance->protocol_data);
        instance->protocol_data = NULL;
    }
}

void nfc_dev_reset(NfcDev* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol_type < NfcProtocolTypeMax);

    if(instance->protocol_data) {
        nfc_protocols[instance->protocol_type]->reset(instance->protocol_data);
    }
}

NfcProtocolType nfc_dev_get_protocol_type(const NfcDev* instance) {
    furi_assert(instance);
    return instance->protocol_type;
}

const NfcProtocolData*
    nfc_dev_get_protocol_data(const NfcDev* instance, NfcProtocolType protocol_type) {
    furi_assert(instance);
    furi_assert(protocol_type < NfcProtocolTypeMax);

    if(instance->protocol_type != protocol_type) {
        furi_crash(NFC_DEV_TYPE_ERROR);
    }

    return instance->protocol_data;
}

const char* nfc_dev_get_protocol_name(const NfcDev* instance, NfcProtocolNameType name_type) {
    furi_assert(instance);
    furi_assert(instance->protocol_type < NfcProtocolTypeMax);

    return nfc_protocols[instance->protocol_type]->get_name(instance->protocol_data, name_type);
}

const uint8_t* nfc_dev_get_uid(const NfcDev* instance, size_t* uid_len) {
    furi_assert(instance);
    furi_assert(instance->protocol_type < NfcProtocolTypeMax);

    return nfc_protocols[instance->protocol_type]->get_uid(instance->protocol_data, uid_len);
}

void nfc_dev_set_protocol_data(
    NfcDev* instance,
    NfcProtocolType protocol_type,
    const NfcProtocolData* protocol_data) {
    furi_assert(instance);
    furi_assert(protocol_type < NfcProtocolTypeMax);

    nfc_dev_clear(instance);

    instance->protocol_type = protocol_type;
    instance->protocol_data = nfc_protocols[protocol_type]->alloc();

    nfc_protocols[protocol_type]->copy(instance->protocol_data, protocol_data);
}

void nfc_dev_copy_protocol_data(
    const NfcDev* instance,
    NfcProtocolType protocol_type,
    NfcProtocolData* protocol_data) {
    furi_assert(instance);
    furi_assert(protocol_type < NfcProtocolTypeMax);
    furi_assert(protocol_data);

    if(instance->protocol_type != protocol_type) {
        furi_crash(NFC_DEV_TYPE_ERROR);
    }

    nfc_protocols[protocol_type]->copy(protocol_data, instance->protocol_data);
}

void nfc_dev_set_loading_callback(NfcDev* instance, NfcLoadingCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->loading_callback = callback;
    instance->loading_callback_context = context;
}

bool nfc_dev_save(NfcDev* instance, const char* path) {
    furi_assert(instance);
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
        if(!flipper_format_write_header_cstr(file, NFC_FILE_HEADER, NFC_CURRENT_FORMAT_VERSION))
            break;

        // Write nfc device type
        if(!flipper_format_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Mifare Classic"))
            break;

        // TODO: Is this really necessary?
        for(NfcProtocolType i = 0; i < NfcProtocolTypeMax; i++) {
            if(instance->protocol_type == i) {
                saved = nfc_protocols[i]->save(
                    instance->protocol_data, file, NFC_CURRENT_FORMAT_VERSION);
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

bool nfc_dev_load(NfcDev* instance, const char* path) {
    furi_assert(instance);
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
        if(furi_string_cmp_str(temp_str, NFC_FILE_HEADER)) break;
        if(version < NFC_LSB_ATQA_FORMAT_VERSION) break;

        // Read Nfc device type
        if(!flipper_format_read_string(file, "Device type", temp_str)) break;

        nfc_dev_clear(instance);

        for(NfcProtocolType i = 0; i < NfcProtocolTypeMax; i++) {
            instance->protocol_type = i;
            instance->protocol_data = nfc_protocols[i]->alloc();

            if(nfc_protocols[i]->verify(instance->protocol_data, temp_str)) {
                loaded = nfc_protocols[i]->load(instance->protocol_data, file, version);
            }

            if(loaded) {
                break;
            } else {
                nfc_protocols[i]->free(instance->protocol_data);
                instance->protocol_data = NULL;
            }
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
