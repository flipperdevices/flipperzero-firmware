#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcDevProtocolNfca,
    // NfcDevProtocolNfcb,
    // NfcDevProtocolNfcf,
    // NfcDevProtocolNfcv,
    NfcDevProtocolIso14443_4a,
    NfcDevProtocolMfUltralight,
    NfcDevProtocolMfClassic,
    NfcDevProtocolMfDesfire,

    NfcDevProtocolNum,
} NfcDevProtocol;

typedef void NfcDevProtocolData;

typedef struct NfcDev NfcDev;

typedef void (*NfcLoadingCallback)(void* context, bool state);

NfcDev* nfc_dev_alloc();

void nfc_dev_free(NfcDev* instance);

void nfc_dev_clear(NfcDev* instance);

void nfc_dev_reset(NfcDev* instance);

NfcDevProtocol nfc_dev_get_protocol(const NfcDev* instance);

const NfcDevProtocolData* nfc_dev_get_data(const NfcDev* instance);

void nfc_dev_set_protocol_data(
    NfcDev* instance,
    NfcDevProtocol protocol,
    const NfcDevProtocolData* protocol_data);

void nfc_dev_set_loading_callback(NfcDev* instance, NfcLoadingCallback callback, void* context);

bool nfc_dev_save(NfcDev* instance, const char* path);

bool nfc_dev_load(NfcDev* instance, const char* path);

#ifdef __cplusplus
}
#endif
