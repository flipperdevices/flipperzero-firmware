#pragma once

#include <furi_hal_nfc.h>
#include "../nfc_worker.h"

#include <m-string.h>

typedef enum {
    NfcSupportedCardTypePlantain,
    NfcSupportedCardTypeTroyka,
    NfcSupportedCardTypePlantain4K,
    NfcSupportedCardTypeTroyka4K,
    NfcSupportedCardTypeTwoCities,
    NfcSupportedCardTypeUnited,

    NfcSupportedCardTypeEnd,
} NfcSupportedCardType;

typedef bool (*NfcSupportedCardVerify)(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

typedef bool (*NfcSupportedCardRead)(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

typedef bool (*NfcSupportedCardParse)(NfcWorker* nfc_worker);

typedef struct {
    NfcProtocol protocol;
    NfcSupportedCardVerify verify;
    NfcSupportedCardRead read;
    NfcSupportedCardParse parse;
} NfcSupportedCard;

extern NfcSupportedCard nfc_supported_card[NfcSupportedCardTypeEnd];
