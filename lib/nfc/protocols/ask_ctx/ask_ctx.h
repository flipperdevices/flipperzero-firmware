#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASK_CTX_UID_SIZE        (4U)
#define ASK_CTX_BLOCK_SIZE      (2U)
#define ASK_CTX_MAX_BLOCK_COUNT (32U)

#define ASK_CTX_PRODUCT_MASK (0xF0U)

#define ASK_CTX_PRODUCT_CTS256B (0x50U)
#define ASK_CTX_PRODUCT_CTX512B (0x60U)

#define ASK_CTX_CTS256B_BLOCK_COUNT (16U)
#define ASK_CTX_CTX512B_BLOCK_COUNT (32U)

typedef enum {
    AskCtxErrorNone,
    AskCtxErrorCommunication,
    AskCtxErrorWrongCrc,
    AskCtxErrorTimeout,
    AskCtxErrorNotPresent,
} AskCtxError;

typedef enum {
    AskCtxTypeUnknown,
    AskCtxTypeCts256B,
    AskCtxTypeCts512B,
    AskCtxTypeCtm512B,
    AskCtxTypeNum,
} AskCtxType;

typedef struct {
    uint8_t uid[ASK_CTX_UID_SIZE];
    uint8_t product_code;
    uint8_t fab_code;
    AskCtxType type;
    uint8_t block_count;
    uint8_t blocks[ASK_CTX_MAX_BLOCK_COUNT][ASK_CTX_BLOCK_SIZE];
} AskCtxData;

extern const NfcDeviceBase nfc_device_ask_ctx;

#ifdef __cplusplus
}
#endif
