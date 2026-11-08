#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INNOVATRON_VT_ADDR_DEFAULT (0x01U)
#define INNOVATRON_DIV_SIZE        (4U)
#define INNOVATRON_ATR_MAX_SIZE    (33U)

typedef enum {
    InnovatronErrorNone,
    InnovatronErrorNotPresent,
    InnovatronErrorCommunication,
    InnovatronErrorWrongCrc,
    InnovatronErrorTimeout,
} InnovatronError;

typedef struct {
    uint8_t vt_addr;
    uint8_t div[INNOVATRON_DIV_SIZE];
    uint8_t verlog;
    uint8_t config;
    uint8_t atr_len;
    uint8_t atr[INNOVATRON_ATR_MAX_SIZE];
} InnovatronData;

extern const NfcDeviceBase nfc_device_innovatron;

InnovatronData* innovatron_alloc(void);

void innovatron_free(InnovatronData* data);

void innovatron_reset(InnovatronData* data);

void innovatron_copy(InnovatronData* data, const InnovatronData* other);

bool innovatron_verify(InnovatronData* data, const FuriString* device_type);

bool innovatron_load(InnovatronData* data, FlipperFormat* ff, uint32_t version);

bool innovatron_save(const InnovatronData* data, FlipperFormat* ff);

bool innovatron_is_equal(const InnovatronData* data, const InnovatronData* other);

const char* innovatron_get_device_name(const InnovatronData* data, NfcDeviceNameType name_type);

const uint8_t* innovatron_get_uid(const InnovatronData* data, size_t* uid_len);

bool innovatron_set_uid(InnovatronData* data, const uint8_t* uid, size_t uid_len);

InnovatronData* innovatron_get_base_data(const InnovatronData* data);

#ifdef __cplusplus
}
#endif
