#pragma once

#include <stdint.h>
#include <lib/nfc/nfc_device.h>

typedef enum {
    ReaderAnalyzerModePcap = 0x01,
    ReaderAnalyzerModeMfkey = 0x02,
} ReaderAnalyzerMode;

typedef struct ReaderAnalyzer ReaderAnalyzer;

typedef void (*ReaderAnalyzerParseDataCallback)(string_t data_str, void* context);

ReaderAnalyzer* reader_analyzer_alloc();

void reader_analyzer_free(ReaderAnalyzer* instance);

void reader_analyzer_set_callback(
    ReaderAnalyzer* instance,
    ReaderAnalyzerParseDataCallback callback,
    void* context);

void reader_analyzer_start(ReaderAnalyzer* instance, ReaderAnalyzerMode mode);

void reader_analyzer_stop(ReaderAnalyzer* instance);

NfcProtocol
    reader_analyzer_guess_protocol(ReaderAnalyzer* instance, uint8_t* buff_rx, uint16_t len);

FuriHalNfcDevData* reader_analyzer_get_nfc_data(ReaderAnalyzer* instance);

void reader_analyzer_prepare_tx_rx(
    ReaderAnalyzer* instance,
    FuriHalNfcTxRxContext* tx_rx,
    bool is_picc);
