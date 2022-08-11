#pragma once

#include <stdint.h>
#include <furi_hal_nfc.h>

typedef struct ReaderAnalyzer ReaderAnalyzer;

void reader_analyzer_reset(ReaderAnalyzer* reader_analyzer);

ReaderAnalyzer* reader_analyzer_alloc();

void reader_analyzer_free(ReaderAnalyzer* reader_analyzer);

bool reader_analyzer_process(ReaderAnalyzer* reader_analyzer, FuriHalNfcTxRxContext* tx_rx);

FuriHalNfcDevData* reader_analyzer_get_nfc_data(ReaderAnalyzer* reader_analyzer);
