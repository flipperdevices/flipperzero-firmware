#pragma once

#include <furi_hal_nfc.h>
#include <storage/storage.h>

typedef struct NfcDebugPcap NfcDebugPcap;

NfcDebugPcap* nfc_debug_pcap_alloc();

void nfc_debug_pcap_free(NfcDebugPcap* instance);

void nfc_debug_pcap_process_data(
    NfcDebugPcap* instance,
    uint8_t* data,
    uint16_t len,
    bool reader_to_tag,
    bool crc_dropped);
