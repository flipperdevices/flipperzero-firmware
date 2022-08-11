#include "reader_analyzer.h"

#include <lib/nfc/protocols/mfkey32v2.h>

#include <m-array.h>

ARRAY_DEF(ReaderAnalyzerMfkeyParams, Mfkey32v2Params, M_POD_OPLIST);

typedef enum {
    ReaderAnalyzerIdle,
    ReaderAnalyzerStateMfClassic,
} ReaderAnalyzerState;

struct ReaderAnalyzer {
    ReaderAnalyzerState state;
    FuriHalNfcDevData nfc_data;
    ReaderAnalyzerMfkeyParams_t mfkey_params;
} ReaderAnalyzer;