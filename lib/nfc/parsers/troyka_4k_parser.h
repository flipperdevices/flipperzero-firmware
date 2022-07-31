#pragma once

#include "nfc_supported_card.h"

bool troyka_4k_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool troyka_4k_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool troyka_4k_parser_parse(NfcWorker* nfc_worker);
