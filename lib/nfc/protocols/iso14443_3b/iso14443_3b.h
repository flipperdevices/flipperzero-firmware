#pragma once

#include <lib/nfc/helpers/bit_buffer.h>
#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_3B_MAX_UID_SIZE (10U)

// TODO change values
#define ISO14443_3B_GUARD_TIME_US (5000)
#define ISO14443_3B_FDT_POLL_FC (1620)
#define ISO14443_3B_FDT_LISTEN_FC (1172)
#define ISO14443_3B_POLLER_MASK_RX_FS ((ISO14443_3B_FDT_LISTEN_FC) / 2)
#define ISO14443_3B_POLL_POLL_MIN_US (1100)

typedef enum {
    Iso14443_3bErrorNone,
    Iso14443_3bErrorNotPresent,
    Iso14443_3bErrorColResFailed,
    Iso14443_3bErrorBufferOverflow,
    Iso14443_3bErrorCommunication,
    Iso14443_3bErrorFieldOff,
    Iso14443_3bErrorWrongCrc,
    Iso14443_3bErrorTimeout,
} Iso14443_3bError;

typedef struct {
    uint8_t uid[ISO14443_3B_MAX_UID_SIZE];
    uint8_t uid_len;
} Iso14443_3bData;

extern const NfcDeviceBase nfc_device_iso14443_3b;

Iso14443_3bData* iso14443_3b_alloc();

void iso14443_3b_free(Iso14443_3bData* data);

void iso14443_3b_reset(Iso14443_3bData* data);

void iso14443_3b_copy(Iso14443_3bData* data, const Iso14443_3bData* other);

bool iso14443_3b_verify(Iso14443_3bData* data, const FuriString* device_type);

bool iso14443_3b_load(Iso14443_3bData* data, FlipperFormat* ff, uint32_t version);

bool iso14443_3b_save(const Iso14443_3bData* data, FlipperFormat* ff);

bool iso14443_3b_is_equal(const Iso14443_3bData* data, const Iso14443_3bData* other);

const char* iso14443_3b_get_device_name(const Iso14443_3bData* data, NfcDeviceNameType name_type);

const uint8_t* iso14443_3b_get_uid(const Iso14443_3bData* data, size_t* uid_len);

const Iso14443_3bData* iso14443_3b_get_base_data(const Iso14443_3bData* data);

#ifdef __cplusplus
}
#endif
