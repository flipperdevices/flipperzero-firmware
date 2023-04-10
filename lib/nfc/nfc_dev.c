#include "nfc_dev.h"

#include <storage/storage.h>

struct NfcDev {
    Storage* storage;
    // DialogsApp* dialogs;
    // NfcDeviceData dev_data;
    // char dev_name[NFC_DEV_NAME_MAX_LEN + 1];
    // FuriString* load_path;
    // FuriString* folder;
    // NfcDeviceSaveFormat format;
    // bool shadow_file_exist;

    // NfcLoadingCallback loading_cb;
    // void* loading_cb_ctx;
};

NfcDev* nfc_dev_alloc() {
    NfcDev* instance = malloc(sizeof(NfcDev));

    return instance;
}

void nfc_dev_free(NfcDev* instance) {
    furi_assert(instance);
}

bool nfc_dev_save(NfcDev* instance, NfcDevData* data, const char* path) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(path);

    bool saved = false;

    return saved;
}

bool nfc_dev_load(NfcDev* instance, NfcDevData* data, const char* path) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(path);

    bool loaded = false;

    return loaded;
}
