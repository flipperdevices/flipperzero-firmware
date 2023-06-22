#pragma once

#include <flipper_format.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcProtocolNameTypeFull,
    NfcProtocolNameTypeShort,
} NfcProtocolNameType;

typedef void NfcProtocolData;

typedef NfcProtocolData* (*NfcProtocolAlloc)();
typedef void (*NfcProtocolFree)(NfcProtocolData* data);
typedef void (*NfcProtocolReset)(NfcProtocolData* data);
typedef void (*NfcProtocolCopy)(NfcProtocolData* data, const NfcProtocolData* other);
typedef bool (*NfcProtocolVerify)(NfcProtocolData* data, const FuriString* device_type);
typedef bool (*NfcProtocolLoad)(NfcProtocolData* data, FlipperFormat* ff, uint32_t version);
typedef bool (*NfcProtocolSave)(const NfcProtocolData* data, FlipperFormat* ff, uint32_t version);
typedef bool (*NfcProtocolEqual)(const NfcProtocolData* data, const NfcProtocolData* other);
typedef const char* (
    *NfcProtocolGetDeviceName)(const NfcProtocolData* data, NfcProtocolNameType name_type);
typedef const uint8_t* (*NfcProtocolGetUid)(const NfcProtocolData* data, size_t* uid_len);

typedef struct {
    const char* protocol_name;
    NfcProtocolAlloc alloc;
    NfcProtocolFree free;
    NfcProtocolReset reset;
    NfcProtocolCopy copy;
    NfcProtocolVerify verify;
    NfcProtocolLoad load;
    NfcProtocolSave save;
    NfcProtocolEqual is_equal;
    NfcProtocolGetDeviceName get_device_name;
    NfcProtocolGetUid get_uid;
} NfcProtocolBase;

#ifdef __cplusplus
}
#endif
