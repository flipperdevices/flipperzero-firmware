#pragma once

#include <nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_4A_CMD_READ_ATS (0xE0)

typedef enum {
    Iso14443_4aErrorNone,
    Iso14443_4aErrorNotPresent,
    Iso14443_4aErrorProtocol,
    Iso14443_4aErrorTimeout,
} Iso14443_4aError;

typedef struct {
    uint8_t tl;
    uint8_t t0;
    uint8_t ta_1;
    uint8_t tb_1;
    uint8_t tc_1;
    uint8_t t1;
} Iso14443_4aAtsData;

typedef struct {
    NfcaData* iso14443_3a_data;
    Iso14443_4aAtsData ats_data;
} Iso14443_4aData;

extern const NfcProtocolBase nfc_protocol_iso14443_4a;

// Virtual methods

Iso14443_4aData* iso14443_4a_alloc();

void iso14443_4a_free(Iso14443_4aData* data);

void iso14443_4a_reset(Iso14443_4aData* data);

void iso14443_4a_copy(Iso14443_4aData* data, const Iso14443_4aData* other);

bool iso14443_4a_verify(Iso14443_4aData* data, const FuriString* device_type);

bool iso14443_4a_load(Iso14443_4aData* data, FlipperFormat* ff, uint32_t version);

bool iso14443_4a_save(const Iso14443_4aData* data, FlipperFormat* ff, uint32_t version);

bool iso14443_4a_is_equal(const Iso14443_4aData* data, const Iso14443_4aData* other);

const char* iso14443_4a_get_name(const Iso14443_4aData* data, NfcProtocolNameType name_type);

const uint8_t* iso14443_4a_get_uid(const Iso14443_4aData* data, size_t* uid_len);

// Getters & Tests

bool iso14443_4a_is_ats_supported(const Iso14443_4aData* data);

#ifdef __cplusplus
}
#endif
