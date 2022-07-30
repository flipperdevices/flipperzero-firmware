#pragma once

#include "nfc_supported_card.h"

bool united_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool united_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool united_parser_parse(NfcWorker* nfc_worker);