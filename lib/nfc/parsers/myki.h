#pragma once

#include "nfc_supported_card.h"

bool myki_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool myki_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx);

bool myki_parser_parse(NfcDeviceData* dev_data);

uint8_t myki_calculate_luhn(uint64_t number);
