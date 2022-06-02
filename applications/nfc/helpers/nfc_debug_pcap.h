#pragma once

#include <furi_hal_nfc.h>
#include <storage/storage.h>

// Prepare tx/rx context for debug pcap logging, if enabled.
// A pcap file will be written using the given storage.
// If is_picc is true, Flipper will be considered PICC for purposes of
// recording packet direction in pcap, else it will be PCD.
void nfc_debug_pcap_prepare_tx_rx(FuriHalNfcTxRxContext* tx_rx, Storage* storage, bool is_picc);
