#pragma once

#include "nfc_device_base.h"

#include <flipper_format.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef NfcDeviceData* (*NfcDeviceAlloc)();
typedef void (*NfcDeviceFree)(NfcDeviceData* data);
typedef void (*NfcDeviceReset)(NfcDeviceData* data);
typedef void (*NfcDeviceCopy)(NfcDeviceData* data, const NfcDeviceData* other);
typedef bool (*NfcDeviceVerify)(NfcDeviceData* data, const FuriString* device_type);
typedef bool (*NfcDeviceLoad)(NfcDeviceData* data, FlipperFormat* ff, uint32_t version);
typedef bool (*NfcDeviceSave)(const NfcDeviceData* data, FlipperFormat* ff);
typedef bool (*NfcDeviceEqual)(const NfcDeviceData* data, const NfcDeviceData* other);
typedef const char* (*NfcDeviceGetName)(const NfcDeviceData* data, NfcDeviceNameType name_type);
typedef const uint8_t* (*NfcDeviceGetUid)(const NfcDeviceData* data, size_t* uid_len);
typedef bool (*NfcDeviceSetUid)(NfcDeviceData* data, const uint8_t* uid, size_t uid_len);
typedef NfcDeviceData* (*NfcDeviceGetBaseData)(const NfcDeviceData* data);

typedef struct {
    const char* protocol_name;
    NfcDeviceAlloc alloc;
    NfcDeviceFree free;
    NfcDeviceReset reset;
    NfcDeviceCopy copy;
    NfcDeviceVerify verify;
    NfcDeviceLoad load;
    NfcDeviceSave save;
    NfcDeviceEqual is_equal;
    NfcDeviceGetName get_name;
    NfcDeviceGetUid get_uid;
    NfcDeviceSetUid set_uid;
    NfcDeviceGetBaseData get_base_data;
} NfcDeviceBase;

#ifdef __cplusplus
}
#endif
