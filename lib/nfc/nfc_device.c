#include "nfc_device.h"

#include <nfc/nfc_common.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#define NFC_FILE_HEADER "Flipper NFC device"
#define NFC_DEV_TYPE_ERROR "Protocol type mismatch"

struct NfcDevice {
    NfcProtocol protocol;
    NfcDeviceData* protocol_data;

    NfcLoadingCallback loading_callback;
    void* loading_callback_context;
};

NfcDevice* nfc_device_alloc() {
    NfcDevice* instance = malloc(sizeof(NfcDevice));

    return instance;
}

void nfc_device_free(NfcDevice* instance) {
    furi_assert(instance);

    nfc_device_clear(instance);
    free(instance);
}

void nfc_device_clear(NfcDevice* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);

    if(instance->protocol_data) {
        nfc_devices[instance->protocol]->free(instance->protocol_data);
        instance->protocol_data = NULL;
    }
}

void nfc_device_reset(NfcDevice* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);

    if(instance->protocol_data) {
        nfc_devices[instance->protocol]->reset(instance->protocol_data);
    }
}

NfcProtocol nfc_device_get_protocol(const NfcDevice* instance) {
    furi_assert(instance);
    return instance->protocol;
}

const NfcDeviceData* nfc_device_get_data(const NfcDevice* instance, NfcProtocol protocol) {
    furi_assert(instance);
    furi_assert(protocol < NfcProtocolNum);

    if(instance->protocol != protocol) {
        furi_crash(NFC_DEV_TYPE_ERROR);
    }

    return instance->protocol_data;
}

const char* nfc_device_get_protocol_name(NfcProtocol protocol) {
    furi_assert(protocol < NfcProtocolNum);

    return nfc_devices[protocol]->protocol_name;
}

const char* nfc_device_get_name(const NfcDevice* instance, NfcDeviceNameType name_type) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);

    return nfc_devices[instance->protocol]->get_name(instance->protocol_data, name_type);
}

const uint8_t* nfc_device_get_uid(const NfcDevice* instance, size_t* uid_len) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);

    return nfc_devices[instance->protocol]->get_uid(instance->protocol_data, uid_len);
}

void nfc_device_set_data(
    NfcDevice* instance,
    NfcProtocol protocol,
    const NfcDeviceData* protocol_data) {
    furi_assert(instance);
    furi_assert(protocol < NfcProtocolNum);

    nfc_device_clear(instance);

    instance->protocol = protocol;
    instance->protocol_data = nfc_devices[protocol]->alloc();

    nfc_devices[protocol]->copy(instance->protocol_data, protocol_data);
}

void nfc_device_copy_data(
    const NfcDevice* instance,
    NfcProtocol protocol,
    NfcDeviceData* protocol_data) {
    furi_assert(instance);
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(protocol_data);

    if(instance->protocol != protocol) {
        furi_crash(NFC_DEV_TYPE_ERROR);
    }

    nfc_devices[protocol]->copy(protocol_data, instance->protocol_data);
}

void nfc_device_set_loading_callback(
    NfcDevice* instance,
    NfcLoadingCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->loading_callback = callback;
    instance->loading_callback_context = context;
}

bool nfc_device_save(NfcDevice* instance, const char* path) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);
    furi_assert(path);

    bool saved = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_buffered_file_alloc(storage);

    FuriString* temp_str;
    temp_str = furi_string_alloc();

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, true);
    }

    do {
        // Open file
        if(!flipper_format_buffered_file_open_always(file, path)) break;

        // Write header
        if(!flipper_format_write_header_cstr(file, NFC_FILE_HEADER, NFC_CURRENT_FORMAT_VERSION))
            break;

        // Write nfc device type
        if(!flipper_format_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Mifare Classic"))
            break;

        saved = nfc_devices[instance->protocol]->save(
            instance->protocol_data, file, NFC_CURRENT_FORMAT_VERSION);

    } while(false);

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, false);
    }

    furi_string_free(temp_str);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return saved;
}

bool nfc_device_load(NfcDevice* instance, const char* path) {
    furi_assert(instance);
    furi_assert(path);

    bool loaded = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_buffered_file_alloc(storage);

    FuriString* temp_str;
    temp_str = furi_string_alloc();

    if(instance->loading_callback) {
        instance->loading_callback(instance->loading_callback_context, true);
    }

    do {
        if(!flipper_format_buffered_file_open_existing(file, path)) break;

        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, NFC_FILE_HEADER)) break;
        if(version < NFC_LSB_ATQA_FORMAT_VERSION) break;

        // Read Nfc device type
        if(!flipper_format_read_string(file, "Device type", temp_str)) break;

        nfc_device_clear(instance);

        for(NfcProtocol i = 0; i < NfcProtocolNum; i++) {
            instance->protocol = i;
            instance->protocol_data = nfc_devices[i]->alloc();

            if(nfc_devices[i]->verify(instance->protocol_data, temp_str)) {
                loaded = nfc_devices[i]->load(instance->protocol_data, file, version);
            }

            if(loaded) {
                break;
            } else {
                nfc_devices[i]->free(instance->protocol_data);
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
