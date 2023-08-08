#pragma once

#include <lib/nfc/helpers/bit_buffer.h>
#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FELICA_UID_SIZE (4U)
#define FELICA_APP_DATA_SIZE (4U)
#define FELICA_PROTOCOL_INFO_SIZE (3U)

#define FELICA_GUARD_TIME_US (5000U)
#define FELICA_FDT_POLL_FC (12000U)
#define FELICA_FDT_ATTRIB_FC (42000U)
#define FELICA_POLL_POLL_MIN_US (1280U)

#define FELICA_REQB_ALL (1U << 3)

#define FELICA_ATTRIB_FRAME_SIZE_32 (0x02)
#define FELICA_ATTRIB_FRAME_SIZE_40 (0x03)
#define FELICA_ATTRIB_FRAME_SIZE_48 (0x04)
#define FELICA_ATTRIB_FRAME_SIZE_64 (0x05)
#define FELICA_ATTRIB_FRAME_SIZE_96 (0x06)
#define FELICA_ATTRIB_FRAME_SIZE_128 (0x07)
#define FELICA_ATTRIB_FRAME_SIZE_256 (0x08)

typedef enum {
    FelicaErrorNone,
    FelicaErrorNotPresent,
    FelicaErrorColResFailed,
    FelicaErrorBufferOverflow,
    FelicaErrorCommunication,
    FelicaErrorFieldOff,
    FelicaErrorWrongCrc,
    FelicaErrorTimeout,
} FelicaError;

typedef struct {
    uint8_t flag;
    uint8_t uid[FELICA_UID_SIZE];
    uint8_t app_data[FELICA_APP_DATA_SIZE];
    uint8_t protocol_info[FELICA_PROTOCOL_INFO_SIZE];
} FelicaAtqB;

typedef struct {
    uint8_t uid[FELICA_UID_SIZE];
    uint8_t app_data[FELICA_APP_DATA_SIZE];
    uint8_t protocol_info[FELICA_PROTOCOL_INFO_SIZE];
} FelicaData;

extern const NfcDeviceBase nfc_device_felica;

FelicaData* felica_alloc();

void felica_free(FelicaData* data);

void felica_reset(FelicaData* data);

void felica_copy(FelicaData* data, const FelicaData* other);

bool felica_verify(FelicaData* data, const FuriString* device_type);

bool felica_load(FelicaData* data, FlipperFormat* ff, uint32_t version);

bool felica_save(const FelicaData* data, FlipperFormat* ff);

bool felica_is_equal(const FelicaData* data, const FelicaData* other);

const char* felica_get_device_name(const FelicaData* data, NfcDeviceNameType name_type);

const uint8_t* felica_get_uid(const FelicaData* data, size_t* uid_len);

bool felica_set_uid(FelicaData* data, const uint8_t* uid, size_t uid_len);

const FelicaData* felica_get_base_data(const FelicaData* data);

#ifdef __cplusplus
}
#endif
