#pragma once

#include <stdbool.h>

#include "protocols/nfc_protocol_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcDev NfcDev;

typedef void (*NfcLoadingCallback)(void* context, bool state);

NfcDev* nfc_dev_alloc();

void nfc_dev_free(NfcDev* instance);

void nfc_dev_clear(NfcDev* instance);

void nfc_dev_reset(NfcDev* instance);

NfcProtocolType nfc_dev_get_protocol_type(const NfcDev* instance);

const NfcProtocolData*
    nfc_dev_get_protocol_data(const NfcDev* instance, NfcProtocolType protocol_type);

const char* nfc_dev_get_protocol_name(const NfcDev* instance);

const uint8_t* nfc_dev_get_uid(const NfcDev* instance, size_t* uid_len);

void nfc_dev_set_protocol_data(
    NfcDev* instance,
    NfcProtocolType protocol_type,
    const NfcProtocolData* protocol_data);

void nfc_dev_copy_protocol_data(
    const NfcDev* instance,
    NfcProtocolType protocol_type,
    NfcProtocolData* protocol_data);

void nfc_dev_set_loading_callback(NfcDev* instance, NfcLoadingCallback callback, void* context);

bool nfc_dev_save(NfcDev* instance, const char* path);

bool nfc_dev_load(NfcDev* instance, const char* path);

#ifdef __cplusplus
}
#endif
