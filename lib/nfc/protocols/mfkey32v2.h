#pragma once

#include "mifare_classic.h"
#include <stddef.h>

typedef struct {
    uint32_t nt;
    uint32_t nr;
    uint32_t ar;
} Mfkey32v2Nonce;

typedef enum {
    Mfkey32v2ParamsStateEmpty,
    Mfkey32v2ParamsStateCollectedPart,
    Mfkey32v2ParamsStateCollectedFull,
    Mfkey32v2ParamsStateSaved,
} Mfkey32v2ParamsState;

typedef struct {
    Mfkey32v2ParamsState state;
    uint8_t sector;
    MfClassicKey key_type;
    uint32_t cuid;
    uint32_t nt0;
    uint32_t nr0;
    uint32_t ar0;
    uint32_t nt1;
    uint32_t nr1;
    uint32_t ar1;
} Mfkey32v2Params;

bool mfkey32v2_collect(FuriHalNfcTxRxContext* tx_rx, Mfkey32v2Params* params);

bool mfkey32v2_get_key(Mfkey32v2Params* params, uint64_t* key);
