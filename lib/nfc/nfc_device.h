#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "protocols/nfc_device_base.h"
#include "protocols/nfc_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcDevice NfcDevice;

typedef void (*NfcLoadingCallback)(void* context, bool state);

NfcDevice* nfc_device_alloc();

void nfc_device_free(NfcDevice* instance);

void nfc_device_clear(NfcDevice* instance);

void nfc_device_reset(NfcDevice* instance);

NfcProtocol nfc_device_get_protocol(const NfcDevice* instance);

const NfcDeviceData* nfc_device_get_data(const NfcDevice* instance, NfcProtocol protocol);

const char* nfc_device_get_protocol_name(NfcProtocol protocol);

const char* nfc_device_get_name(const NfcDevice* instance, NfcDeviceNameType name_type);

const uint8_t* nfc_device_get_uid(const NfcDevice* instance, size_t* uid_len);

bool nfc_device_set_uid(NfcDevice* instance, const uint8_t* uid, size_t uid_len);

void nfc_device_set_data(
    NfcDevice* instance,
    NfcProtocol protocol,
    const NfcDeviceData* protocol_data);

void nfc_device_copy_data(
    const NfcDevice* instance,
    NfcProtocol protocol,
    NfcDeviceData* protocol_data);

bool nfc_device_is_equal_data(
    const NfcDevice* instance,
    NfcProtocol protocol,
    const NfcDeviceData* protocol_data);

bool nfc_device_is_equal(const NfcDevice* instance, const NfcDevice* other);

void nfc_device_set_loading_callback(
    NfcDevice* instance,
    NfcLoadingCallback callback,
    void* context);

bool nfc_device_save(NfcDevice* instance, const char* path);

bool nfc_device_load(NfcDevice* instance, const char* path);

#ifdef __cplusplus
}
#endif
