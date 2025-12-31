#pragma once

#include "nfc_logger.h"
#include "transaction/nfc_transaction_i.h"
#include <nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcLoggerStateDisabled,
    NfcLoggerStateIdle,
    NfcLoggerStateProcessing,
    NfcLoggerStateStopped,
    NfcLoggerStateError
} NfcLoggerState;

NfcLogger* nfc_logger_alloc(void);
void nfc_logger_free(NfcLogger* instance);

bool nfc_logger_enabled(NfcLogger* instance);
void nfc_logger_set_protocol(NfcLogger* instance, NfcProtocol protocol);

void nfc_logger_start(NfcLogger* instance, NfcMode mode);
void nfc_logger_stop(NfcLogger* instance);

void nfc_logger_transaction_begin(NfcLogger* instance, FuriHalNfcEvent event);
void nfc_logger_transaction_end(NfcLogger* instance);

void nfc_logger_append_request_data(
    NfcLogger* instance,
    const uint8_t* data,
    const size_t data_size);

void nfc_logger_append_response_data(
    NfcLogger* instance,
    const uint8_t* data,
    const size_t data_size);

void nfc_logger_append_history(NfcLogger* instance, NfcHistoryItem* history);

void nfc_logger_set_poller_log_callback(
    NfcLogger* instance,
    NfcGenericInstance* poller,
    NfcGenericLogHistoryCallback callback);
#ifdef __cplusplus
}
#endif
