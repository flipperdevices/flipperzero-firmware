#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcLoggerTransactionFilterAll,
    NfcLoggerTransactionFilterPayloadOnly,
} NfcLoggerTransactionFilter;

typedef enum {
    NfcLoggerHistoryLayerFilterAll,
    NfcLoggerHistoryLayerFilterTopProtocol,
    NfcLoggerHistoryLayerFilterTopProtocolSimplified
} NfcLoggerHistoryLayerFilter;

typedef struct {
    NfcLoggerTransactionFilter transaction_filter;
    NfcLoggerHistoryLayerFilter history_filter;
} NfcLoggerFormatterConfig;

#ifdef __cplusplus
}
#endif
