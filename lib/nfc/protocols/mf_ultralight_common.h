#pragma once

#include "nfca_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MF_ULTRALIGHT_CMD_GET_VERSION (0x60)
#define MF_ULTRALIGHT_CMD_READ_PAGE (0x30)
#define MF_ULTRALIGHT_CMD_WRITE_PAGE (0xA2)
#define MF_ULTRALIGHT_CMD_ACK (0x0A)
#define MF_ULTRALIGTH_CMD_READ_SIG (0x3C)
#define MF_ULTRALIGHT_CMD_READ_CNT (0x39)
#define MF_ULTRALIGHT_CMD_CHECK_TEARING (0x3E)

#define MF_ULTRALIGHT_MAX_PAGE_NUM (510)
#define MF_ULTRALIGHT_PAGE_SIZE (4U)
#define MF_ULTRALIGHT_SIGNATURE_SIZE (32)
#define MF_ULTRALIGHT_COUNTER_SIZE (3)
#define MF_ULTRALIGHT_COUNTER_NUM (3)
#define MF_ULTRALIGHT_TEARING_FLAG_SIZE (1)
#define MF_ULTRALIGHT_TEARING_FLAG_NUM (3)

typedef enum {
    MfUltralightErrorNone,
    MfUltralightErrorNotPresent,
    MfUltralightErrorProtocol,
    MfUltralightErrorAuth,
    MfUltralightErrorTimeout,
} MfUltralightError;

typedef enum {
    MfUltralightTypeUnknown,
    MfUltralightTypeNTAG203,
    // Below have config pages and GET_VERSION support
    MfUltralightTypeUL11,
    MfUltralightTypeUL21,
    MfUltralightTypeNTAG213,
    MfUltralightTypeNTAG215,
    MfUltralightTypeNTAG216,
    // Below also have sector select
    // NTAG I2C's *does not* have regular config pages, so it's a bit of an odd duck
    MfUltralightTypeNTAGI2C1K,
    MfUltralightTypeNTAGI2C2K,
    // NTAG I2C Plus has stucture expected from NTAG21x
    MfUltralightTypeNTAGI2CPlus1K,
    MfUltralightTypeNTAGI2CPlus2K,

    // Keep last for number of types calculation
    MfUltralightTypeNum,
} MfUltralightType;

typedef struct {
    uint8_t data[MF_ULTRALIGHT_PAGE_SIZE];
} MfUltralightPage;

typedef struct {
    uint8_t header;
    uint8_t vendor_id;
    uint8_t prod_type;
    uint8_t prod_subtype;
    uint8_t prod_ver_major;
    uint8_t prod_ver_minor;
    uint8_t storage_size;
    uint8_t protocol_type;
} MfUltralightVersion;

typedef struct {
    uint8_t data[MF_ULTRALIGHT_SIGNATURE_SIZE];
} MfUltralightSignature;

typedef struct {
    uint8_t data[MF_ULTRALIGHT_COUNTER_SIZE];
} MfUltralightCounter;

typedef struct {
    uint8_t data[MF_ULTRALIGHT_TEARING_FLAG_SIZE];
} MfUltralightTearingFlag;

typedef struct {
    NfcaData nfca_data;
    MfUltralightVersion version;
    MfUltralightSignature signature;
    MfUltralightCounter counter[MF_ULTRALIGHT_COUNTER_NUM];
    MfUltralightTearingFlag tearing_flag[MF_ULTRALIGHT_TEARING_FLAG_NUM];
    MfUltralightPage page[MF_ULTRALIGHT_MAX_PAGE_NUM];
} MfUltralightData;

#ifdef __cplusplus
}
#endif
