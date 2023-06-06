#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

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
    uint8_t cmd;
    uint8_t param;
} Iso14443_4aAtsRequest;

typedef struct {
    uint8_t tl;
    uint8_t t0;
    uint8_t ta_1;
    uint8_t tb_1;
    uint8_t tc_1;
    uint8_t t1;
} Iso14443_4aAtsResponse;

typedef struct {
    NfcaData iso14443_3a_data;
} Iso14443_4aData;

#ifdef __cplusplus
}
#endif
