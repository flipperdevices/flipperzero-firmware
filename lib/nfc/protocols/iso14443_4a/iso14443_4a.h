#pragma once

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>

#include <lib/toolbox/simple_array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_4aErrorNone,
    Iso14443_4aErrorNotPresent,
    Iso14443_4aErrorProtocol,
    Iso14443_4aErrorTimeout,
} Iso14443_4aError;

typedef struct {
    Iso14443_3aData* iso14443_3a_data;
    SimpleArray* ats_data;
} Iso14443_4aData;

// Virtual methods

Iso14443_4aData* iso14443_4a_alloc();

void iso14443_4a_free(Iso14443_4aData* data);

void iso14443_4a_reset(Iso14443_4aData* data);

void iso14443_4a_copy(Iso14443_4aData* data, const Iso14443_4aData* other);

bool iso14443_4a_verify(Iso14443_4aData* data, const FuriString* device_type);

bool iso14443_4a_load(Iso14443_4aData* data, FlipperFormat* ff, uint32_t version);

bool iso14443_4a_save(const Iso14443_4aData* data, FlipperFormat* ff);

bool iso14443_4a_is_equal(const Iso14443_4aData* data, const Iso14443_4aData* other);

const char* iso14443_4a_get_device_name(const Iso14443_4aData* data, NfcDeviceNameType name_type);

const uint8_t* iso14443_4a_get_uid(const Iso14443_4aData* data, size_t* uid_len);

bool iso14443_4a_set_uid(Iso14443_4aData* data, const uint8_t* uid, size_t uid_len);

Iso14443_3aData* iso14443_4a_get_base_data(const Iso14443_4aData* data);

// Getters & Tests

bool iso14443_4a_is_ats_supported(const Iso14443_4aData* data);

#ifdef __cplusplus
}
#endif
