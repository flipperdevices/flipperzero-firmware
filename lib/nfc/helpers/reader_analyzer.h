#pragma once

#include <stdint.h>
#include <lib/nfc/nfc_device.h>

typedef struct ReaderAnalyzer ReaderAnalyzer;

typedef void (*ReaderAnalyzerParseDataCallback)(string_t data_str, void* context);

ReaderAnalyzer* reader_analyzer_alloc(Storage* storage);

void reader_analyzer_free(ReaderAnalyzer* reader_analyzer);

void reader_analyzer_set_callback(
    ReaderAnalyzer* reader_analyzer,
    ReaderAnalyzerParseDataCallback callback,
    void* context);

NfcProtocol
    reader_analyzer_guess_protocol(ReaderAnalyzer* reader_analyzer, uint8_t* buff_rx, uint16_t len);

FuriHalNfcDevData* reader_analyzer_get_nfc_data(ReaderAnalyzer* reader_analyzer);

void reader_analyzer_prepare_tx_rx(
    ReaderAnalyzer* reader_analyzer,
    FuriHalNfcTxRxContext* tx_rx,
    bool is_picc);
