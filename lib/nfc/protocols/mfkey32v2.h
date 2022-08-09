#pragma once

#include "mifare_classic.h"

typedef struct {
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
