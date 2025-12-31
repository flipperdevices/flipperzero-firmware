#pragma once

#include <furi_hal_rtc.h>
#include <toolbox/path.h>

#include "nfc_logger_i.h"
#include "nfc_trace_data_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NfcLogger {
    uint8_t trace_count;
    bool skip_empty_transactions;
    NfcLoggerState state;
    NfcProtocol protocol;
    NfcTrace* trace;
    NfcTransaction* transaction;
    uint8_t max_chain_size;
    uint8_t history_size_bytes;

    uint32_t dwt_cnt_prev;
    uint32_t dwt_ovf_cnt;
    uint32_t dwt_second_per_ovf;
    uint32_t dwt_cycles_per_timeout_delay;
    FuriMutex* dwt_mutex;

    FuriString* log_folder_path;
    FuriString* filename;
    Storage* storage;
    FuriThread* logger_thread;
    FuriMessageQueue* transaction_queue;
    NfcGenericInstance* log_context;
    NfcGenericLogHistoryCallback log_callback;

    bool exit;
};

#ifdef __cplusplus
}
#endif
