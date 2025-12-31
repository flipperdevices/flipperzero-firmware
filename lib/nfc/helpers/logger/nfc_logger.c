#include "nfc_logger_context.h"
#include "history/nfc_history_size.h"
#include <nfc_device.h>
#include <furi_hal_cortex.h>

#define TAG "NfcLogger"

#define NFC_LOG_TEMP_FILE_NAME           "log_temp.bin"
#define NFC_LOG_MESSAGE_QUEUE_TIMEOUT_MS (50)
#define NFC_LOG_MESSAGE_QUEUE_TIMEOUT_US (NFC_LOG_MESSAGE_QUEUE_TIMEOUT_MS * 1000U)

#define NFC_LOG_DWT_CYCCNT_CYCLES_PER_MESSAGE_QUEUE_TIMEOUT(inst_per_us) \
    (NFC_LOG_MESSAGE_QUEUE_TIMEOUT_US * (inst_per_us))
#define NFC_LOG_DWT_CYCCNT_SECONDS_MAX(inst_per_us) (UINT32_MAX / (inst_per_us))

static uint32_t nfc_logger_get_time(NfcLogger* instance) {
    uint32_t time = 0;
    uint32_t inst_per_us = furi_hal_cortex_instructions_per_microsecond();

    if(furi_mutex_acquire(instance->dwt_mutex, 10) == FuriStatusOk) {
        uint32_t dwt_prev = instance->dwt_cnt_prev;

        uint32_t dwt = DWT->CYCCNT;
        time = (dwt < dwt_prev) ? ((UINT32_MAX - dwt_prev) + dwt) / inst_per_us :
                                  (dwt - dwt_prev) / inst_per_us;

        time += instance->dwt_second_per_ovf * instance->dwt_ovf_cnt;
        furi_mutex_release(instance->dwt_mutex);
    } else {
        FURI_LOG_W(TAG, "Unable to acquire time mutex");
    }
    return time;
}

static inline void nfc_logger_check_dwt_overflow(NfcLogger* instance) {
    if(furi_mutex_acquire(instance->dwt_mutex, 10) == FuriStatusOk) {
        if(instance->dwt_cnt_prev - DWT->CYCCNT < instance->dwt_cycles_per_timeout_delay) {
            while(instance->dwt_cnt_prev > DWT->CYCCNT)
                ;
            instance->dwt_ovf_cnt += 1;
            instance->dwt_cnt_prev = DWT->CYCCNT;
        }
        furi_mutex_release(instance->dwt_mutex);
    } else {
        FURI_LOG_W(TAG, "Unable to check DWT overflow");
    }
}

static bool nfc_logger_trace_save(Stream* stream, NfcTrace* trace) {
    bool result = false;
    do {
        if(!stream_seek(stream, 0, StreamOffsetFromStart)) {
            FURI_LOG_E(TAG, "Seek failed");
            break;
        }

        size_t bytes_to_write = sizeof(NfcTrace);
        if(stream_write(stream, (uint8_t*)trace, bytes_to_write) != bytes_to_write) {
            FURI_LOG_E(TAG, "Unable to save trace");
            break;
        }

        result = true;
    } while(false);
    return result;
}

static int32_t nfc_logger_thread_callback(void* context) {
    FURI_LOG_D(TAG, "Thread start");
    NfcLogger* instance = context;

    Stream* stream = file_stream_alloc(instance->storage);
    FuriString* temp_file_path = furi_string_alloc_set(instance->log_folder_path);
    path_append(temp_file_path, NFC_LOG_TEMP_FILE_NAME);

    do {
        if(!file_stream_open(
               stream, furi_string_get_cstr(temp_file_path), FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
            instance->state = NfcLoggerStateError;
            FURI_LOG_E(TAG, "Unable to create temp log file");
            break;
        }

        if(!nfc_logger_trace_save(stream, instance->trace)) {
            instance->state = NfcLoggerStateError;
            break;
        }

        while(!instance->exit || furi_message_queue_get_count(instance->transaction_queue)) {
            nfc_logger_check_dwt_overflow(instance);

            NfcTransaction* ptr = NULL;
            if(furi_message_queue_get(
                   instance->transaction_queue, &ptr, NFC_LOG_MESSAGE_QUEUE_TIMEOUT_MS) ==
               FuriStatusErrorTimeout)
                continue;

            if(!nfc_transaction_save(stream, ptr)) {
                instance->state = NfcLoggerStateError;
                instance->exit = true;
            }

            nfc_transaction_free(ptr);
        }

        if(!nfc_logger_trace_save(stream, instance->trace)) {
            instance->state = NfcLoggerStateError;
            break;
        }
    } while(false);

    stream_free(stream);
    furi_string_free(temp_file_path);

    if(instance->state == NfcLoggerStateError)
        FURI_LOG_E(TAG, "Logger thread stopped due to an error");
    return 0;
}

static bool nfc_logger_make_log_folder(Storage* storage, const char* log_folder_path) {
    furi_assert(storage);
    bool result = true;
    if(!storage_simply_mkdir(storage, log_folder_path)) {
        FURI_LOG_W(TAG, "Unable to create log folder: %s", log_folder_path);
        result = false;
    }
    return result;
}

NfcLogger* nfc_logger_alloc(void) {
    NfcLogger* instance = malloc(sizeof(NfcLogger));

    instance->storage = furi_record_open(RECORD_STORAGE);
    instance->filename = furi_string_alloc();
    instance->log_folder_path = furi_string_alloc();
    instance->dwt_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    instance->transaction_queue = furi_message_queue_alloc(150, sizeof(NfcTransaction*));

    FuriThread* thread = furi_thread_alloc_ex(TAG, 1024U, nfc_logger_thread_callback, instance);
    furi_thread_set_priority(thread, FuriThreadPriorityLow);
    instance->logger_thread = thread;

    uint32_t inst_per_us = furi_hal_cortex_instructions_per_microsecond();
    instance->dwt_cycles_per_timeout_delay =
        NFC_LOG_DWT_CYCCNT_CYCLES_PER_MESSAGE_QUEUE_TIMEOUT(inst_per_us);
    instance->dwt_second_per_ovf = NFC_LOG_DWT_CYCCNT_SECONDS_MAX(inst_per_us);
    return instance;
}

void nfc_logger_free(NfcLogger* instance) {
    furi_assert(instance);

    furi_mutex_free(instance->dwt_mutex);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(instance->filename);
    furi_string_free(instance->log_folder_path);
    furi_message_queue_free(instance->transaction_queue);
    furi_thread_free(instance->logger_thread);

    free(instance);
}

void nfc_logger_config(NfcLogger* instance, bool enabled, const char* log_folder_path) {
    furi_assert(instance);
    furi_assert(log_folder_path);

    if(enabled) {
        instance->state = NfcLoggerStateIdle;

        if(!nfc_logger_make_log_folder(instance->storage, log_folder_path)) {
            instance->state = NfcLoggerStateError;
        } else {
            furi_string_set_str(instance->log_folder_path, log_folder_path);
        }
    } else {
        instance->state = NfcLoggerStateDisabled;
    }
}

bool nfc_logger_raw_log_file_present(NfcLogger* instance, FuriString* output) {
    furi_assert(instance);
    furi_assert(output);
    bool result = false;

    File* f = storage_file_alloc(instance->storage);

    if(storage_dir_open(f, furi_string_get_cstr(instance->log_folder_path))) {
        FileInfo f_info;
        char name[50];
        memset(name, 0, sizeof(name));

        while(storage_dir_read(f, &f_info, name, sizeof(name))) {
            if(f_info.flags & FSF_DIRECTORY) {
                continue;
            }

            if(strcmp(name, NFC_LOG_TEMP_FILE_NAME) == 0) continue;
            char* file_ext = strstr(name, ".bin");
            if((file_ext == NULL) || (strcmp(file_ext, ".bin") != 0)) {
                continue;
            }

            *file_ext = '\0';
            furi_string_set(output, instance->log_folder_path);
            path_append(output, name);
            furi_string_cat_str(output, ".txt");
            if(storage_file_exists(instance->storage, furi_string_get_cstr(output))) {
                furi_string_reset(output);
                continue;
            } else {
                size_t extension_index = furi_string_search_rchar(output, '.');
                furi_string_left(output, extension_index);
                result = true;
                break;
            }
        }
        storage_dir_close(f);
    }
    storage_file_free(f);
    return result;
}

static NfcTrace* nfc_logger_trace_alloc(NfcProtocol protocol, NfcMode mode) {
    NfcTrace* trace = malloc(sizeof(NfcTrace));
    trace->mode = mode;
    trace->protocol = protocol;
    return trace;
}

static void nfc_logger_trace_free(NfcTrace* trace) {
    furi_assert(trace);
    free(trace);
}

static inline void nfc_logger_wait_for_free_memory(NfcLogger* instance) {
    while(memmgr_heap_get_max_free_block() <
          (nfc_transaction_get_size(instance->history_size_bytes) * 10)) {
        furi_delay_ms(5);
    }
}

bool nfc_logger_enabled(NfcLogger* instance) {
    furi_assert(instance);
    return instance->state != NfcLoggerStateDisabled && instance->state != NfcLoggerStateError;
}

void nfc_logger_set_protocol(NfcLogger* instance, NfcProtocol protocol) {
    furi_assert(instance);
    furi_assert(protocol < NfcProtocolNum);
    instance->protocol = protocol;
    instance->skip_empty_transactions = protocol == NfcProtocolMfClassic;
}

void nfc_logger_start(NfcLogger* instance, NfcMode mode) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);
    furi_assert(mode < NfcModeNum);

    if(instance->state == NfcLoggerStateDisabled) return;

    instance->max_chain_size = 2;
    instance->history_size_bytes =
        nfc_history_get_size_bytes(instance->protocol, mode, instance->max_chain_size);

    instance->exit = false;
    instance->trace = nfc_logger_trace_alloc(instance->protocol, mode);

    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    furi_string_printf(
        instance->filename,
        "LOG-%.4d%.2d%.2d-%.2d%.2d%.2d-%d",
        dt.year,
        dt.month,
        dt.day,
        dt.hour,
        dt.minute,
        dt.second,
        instance->trace_count);

    furi_thread_start(instance->logger_thread);
    instance->dwt_cnt_prev = DWT->CYCCNT;
    instance->dwt_ovf_cnt = 0;
}

void nfc_logger_stop(NfcLogger* instance) {
    furi_assert(instance);

    if(instance->state == NfcLoggerStateDisabled) return;

    if(instance->state != NfcLoggerStateError) {
        nfc_logger_transaction_end(instance);
    }

    instance->exit = true;
    furi_thread_join(instance->logger_thread);

    if(instance->state != NfcLoggerStateError) {
        FuriString* temp_file_path = furi_string_alloc_set(instance->log_folder_path);
        path_append(temp_file_path, NFC_LOG_TEMP_FILE_NAME);

        FuriString* str = furi_string_alloc_set(instance->log_folder_path);
        path_append(str, furi_string_get_cstr(instance->filename));
        furi_string_cat_str(str, ".bin");

        FS_Error status = storage_common_copy(
            instance->storage, furi_string_get_cstr(temp_file_path), furi_string_get_cstr(str));
        ///TODO: Maybe rename with storage_common_rename(instance->storage) would be better?

        furi_string_free(str);
        furi_string_free(temp_file_path);
        UNUSED(status);

        instance->state = NfcLoggerStateStopped;
        instance->trace_count++;
    }

    FURI_LOG_I(
        TAG,
        "Saved %lu transacrions to %s",
        (uint32_t)instance->trace->transactions_count,
        furi_string_get_cstr(instance->filename));

    nfc_logger_trace_free(instance->trace);
}

void nfc_logger_transaction_begin(NfcLogger* instance, FuriHalNfcEvent event) {
    furi_assert(instance);

    if(instance->state == NfcLoggerStateDisabled || instance->state == NfcLoggerStateError) return;

    bool update_existing = false;
    if(instance->transaction) {
        NfcTransactionType type = nfc_transaction_get_type(instance->transaction);
        if(type != NfcTransactionTypeEmpty)
            nfc_logger_transaction_end(instance);
        else
            update_existing = true;
    }

    uint32_t id = instance->trace->transactions_count;
    instance->state = NfcLoggerStateProcessing;
    if(!update_existing) {
        nfc_logger_wait_for_free_memory(instance);
        uint32_t time = nfc_logger_get_time(instance);
        instance->transaction = nfc_transaction_alloc(
            id, event, time, instance->history_size_bytes, instance->max_chain_size);
    } else {
        uint32_t time = nfc_logger_get_time(instance);
        nfc_transaction_refresh(instance->transaction, id, event, time);
    }
}

void nfc_logger_transaction_end(NfcLogger* instance) {
    furi_assert(instance);
    do {
        if(instance->state == NfcLoggerStateDisabled || instance->state == NfcLoggerStateError)
            return;
        if(instance->state != NfcLoggerStateProcessing) break;
        if(!instance->transaction) break;
        if((nfc_transaction_get_type(instance->transaction) == NfcTransactionTypeEmpty) &&
           instance->skip_empty_transactions)
            break;

        if(instance->trace->mode == NfcModePoller) {
            instance->log_callback(instance, instance->log_context);
        }

        uint32_t time = nfc_logger_get_time(instance);
        nfc_transaction_complete(instance->transaction, time);

        if(furi_message_queue_put(instance->transaction_queue, &instance->transaction, 10) !=
           FuriStatusOk) {
            instance->state = NfcLoggerStateError;
            FURI_LOG_E(TAG, "Transaction lost, logger will be stopped!");
            nfc_logger_stop(instance);
        }

        instance->trace->transactions_count++;
        instance->transaction = NULL;
    } while(false);
}

void nfc_logger_append_request_data(
    NfcLogger* instance,
    const uint8_t* data,
    const size_t data_size) {
    furi_assert(instance);
    furi_assert(data);

    do {
        if(instance->state == NfcLoggerStateDisabled || instance->state == NfcLoggerStateError)
            break;
        if(data_size == 0) break;

        bool begin_new_transaction = false;
        if(instance->transaction) {
            NfcTransactionType type = nfc_transaction_get_type(instance->transaction);
            begin_new_transaction =
                (type == NfcTransactionTypeRequest || type == NfcTransactionTypeRequestResponse);
        } else {
            begin_new_transaction = true;
        }

        if(begin_new_transaction) {
            nfc_logger_transaction_begin(instance, FuriHalNfcEventTxStart);
        }

        uint32_t time = nfc_logger_get_time(instance);
        nfc_transaction_append(instance->transaction, time, data, data_size, false);
    } while(false);
}

void nfc_logger_append_response_data(
    NfcLogger* instance,
    const uint8_t* data,
    const size_t data_size) {
    furi_assert(instance);
    furi_assert(data);

    do {
        if(instance->state == NfcLoggerStateDisabled || instance->state == NfcLoggerStateError)
            break;
        if(data_size == 0) break;

        uint32_t time = nfc_logger_get_time(instance);
        nfc_transaction_append(instance->transaction, time, data, data_size, true);
    } while(false);
}

void nfc_logger_append_history(NfcLogger* instance, NfcHistoryItem* history) {
    furi_assert(instance);
    furi_assert(history);

    if(instance->state == NfcLoggerStateDisabled || instance->state == NfcLoggerStateError) return;
    if(!history->base.modified) return;

    history->base.modified = false;
    if(instance->transaction) {
        nfc_transaction_append_history(instance->transaction, history);
    }
}

void nfc_logger_set_poller_log_callback(
    NfcLogger* instance,
    NfcGenericInstance* poller,
    NfcGenericLogHistoryCallback callback) {
    furi_assert(instance);
    furi_assert(poller);
    furi_assert(callback);
    instance->log_context = poller;
    instance->log_callback = callback;
}
