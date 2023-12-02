#pragma once

#include <stdint.h>
#include <lib/nfc/nfc_device.h>

typedef enum {
    ReaderAnalyzerModeDebugLog = 0x01,
    ReaderAnalyzerModeDebugPcap = 0x04,
} ReaderAnalyzerMode;

typedef enum {
    ReaderAnalyzerEventMfkeyCollected,
} ReaderAnalyzerEvent;

typedef struct ReaderAnalyzer ReaderAnalyzer;

typedef void (*ReaderAnalyzerParseDataCallback)(ReaderAnalyzerEvent event, void* context);

ReaderAnalyzer* reader_analyzer_alloc();

void reader_analyzer_free(ReaderAnalyzer* instance);

void reader_analyzer_set_callback(
    ReaderAnalyzer* instance,
    ReaderAnalyzerParseDataCallback callback,
    void* context);

void reader_analyzer_start(ReaderAnalyzer* instance, ReaderAnalyzerMode mode);

void reader_analyzer_stop(ReaderAnalyzer* instance);

void reader_analyzer_prepare_tx_rx(
    ReaderAnalyzer* instance,
    FuriHalNfcTxRxContext* tx_rx,
    bool is_picc);
