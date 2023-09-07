#pragma once

#include <toolbox/bit_buffer.h>
#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_3B_UID_SIZE (4U)
#define ISO14443_3B_APP_DATA_SIZE (4U)

#define ISO14443_3B_GUARD_TIME_US (5000U)
#define ISO14443_3B_FDT_POLL_FC (9000U)
#define ISO14443_3B_FDT_ATTRIB_FC (42000U)
#define ISO14443_3B_POLL_POLL_MIN_US (1280U)

#define ISO14443_3B_REQB_ALL (1U << 3)

#define ISO14443_3B_ATTRIB_FRAME_SIZE_32 (0x02)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_40 (0x03)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_48 (0x04)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_64 (0x05)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_96 (0x06)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_128 (0x07)
#define ISO14443_3B_ATTRIB_FRAME_SIZE_256 (0x08)

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
    uint8_t bit_rate_capability;
    uint8_t max_frame_size : 4;
    uint8_t protocol_type : 4;
    uint8_t fwi : 4;
    uint8_t rfu : 2;
    uint8_t fo : 2;
} Iso14443_3bProtocolInfo;

typedef struct {
    uint8_t uid[ISO14443_3B_UID_SIZE];
    uint8_t app_data[ISO14443_3B_APP_DATA_SIZE];
    Iso14443_3bProtocolInfo protocol_info;
} Iso14443_3bData;

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

bool iso14443_3b_set_uid(Iso14443_3bData* data, const uint8_t* uid, size_t uid_len);

Iso14443_3bData* iso14443_3b_get_base_data(const Iso14443_3bData* data);

// Getters and tests

bool iso14443_3b_supports_iso14443_4(const Iso14443_3bData* data);

#ifdef __cplusplus
}
#endif
