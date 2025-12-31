
#pragma once

#include "nfc_history_data_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t NfcChainLength;

typedef struct {
    NfcChainLength length;
    NfcHistoryItem items[];
} FURI_PACKED NfcHistoryChain;

typedef struct {
    uint8_t chain_count;
    uint8_t max_chain_count;
    uint8_t history_size_bytes;
} NfcHistoryBase;

struct FURI_PACKED NfcHistory {
    NfcHistoryBase base;
    NfcHistoryChain chains[];
};

#ifdef __cplusplus
}
#endif
