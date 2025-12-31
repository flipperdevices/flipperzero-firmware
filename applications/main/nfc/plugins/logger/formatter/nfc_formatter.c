#include <furi.h>
#include <nfc/helpers/logger/nfc_trace_data_type.h>
#include <nfc/helpers/logger/transaction/nfc_transaction.h>

#include <nfc/helpers/logger/nfc_trace_data_type.h>
#include <nfc/helpers/logger/transaction/nfc_transaction_data_type.h>
#include <nfc/helpers/logger/history/nfc_history_chain_data_type.h>

#include <flipper_application/flipper_application.h>
#include <toolbox/path.h>

#include "nfc_formatter_context.h"
#include "../nfc_logger_formatter_plugin.h"

#include "common/nfc_transaction_formatter.h"
#include "protocols/nfc_protocol_formatters.h"

#define TAG "NfcLoggerFormatter"

static void
    nfc_format_trace_protocol_layers_description(NfcProtocol trace_protocol, FuriString* output) {
    furi_assert(output);
    furi_assert(trace_protocol < NfcProtocolNum);

    furi_string_cat_printf(output, "Protocol layers:\r\n");

    uint8_t count = 0;
    NfcProtocol* protocols = nfc_protocol_layer_list_alloc(trace_protocol, &count);
    furi_string_cat_printf(output, "L0 - NFC\r\n");
    for(uint8_t i = 0; i < count; i++) {
        furi_string_cat_printf(
            output, "L%d - %s\r\n", i + 1, nfc_device_get_protocol_name(protocols[i]));
    }
    furi_string_cat_printf(output, "\r\n");
    nfc_protocol_layer_list_free(protocols);
}

static void nfc_format_trace(
    NfcFormatter* instance,
    FuriString* file_path,
    const NfcTrace* trace,
    FuriString* output) {
    furi_assert(instance);
    furi_assert(file_path);
    furi_assert(trace);
    furi_assert(output);

    instance->protocol = trace->protocol;
    instance->mode = trace->mode;

    FuriString* file_name = furi_string_alloc();
    path_extract_filename(file_path, file_name, true);

    furi_string_printf(output, "Trace info\nName: %s\n", furi_string_get_cstr(file_name));
    furi_string_free(file_name);

    furi_string_cat_printf(
        output,
        "Protocol: %s\nMode: %s\nTransaction count: %lu\n\n",
        nfc_device_get_protocol_name(trace->protocol),
        trace->mode == NfcModeListener ? "NfcListener" : "NfcPoller",
        (uint32_t)trace->transactions_count);

    nfc_format_trace_protocol_layers_description(trace->protocol, output);
}

static NfcFormatter* nfc_formatter_alloc(const NfcLoggerFormatterConfig* config) {
    NfcFormatter* instance = malloc(sizeof(NfcFormatter));

    instance->config = config;
    const size_t width[] = {5, 10, 11, 3, 60, 3, 120};
    const char* names[] = {"Id", "Type", "Time", "Src", "Data", "CRC", "Annotation"};
    instance->table_columns_cnt = COUNT_OF(width);
    instance->table = table_alloc(instance->table_columns_cnt, width, names);
    table_set_column_alignment(instance->table, 4, TableColumnDataAlignmentLeft);

    instance->protocol = NfcProtocolInvalid;
    return instance;
}

static void nfc_formatter_free(NfcFormatter* instance) {
    furi_assert(instance);
    table_free(instance->table);
    free(instance);
}

static void nfc_format_table_header(const NfcFormatter* instance, FuriString* output) {
    furi_assert(instance);
    table_printf_header(instance->table, output);
}

static NfcHistoryCrcStatus
    nfc_history_find_crc_from_chain(const NfcHistoryChain* chain, const NfcMode mode) {
    NfcHistoryCrcStatus status = NfcHistoryCrcNotSet;
    for(size_t i = 0; i < chain->length; i++) {
        NfcProtocol protocol = chain->items[i].base.protocol;
        const NfcProtocolFormatterBase* formatter = nfc_protocol_formatter_get(protocol, mode);
        if(!formatter) break;

        const NfcHistoryGetCrcStatusCallback crc_callback = formatter->get_crc_status;
        if(!crc_callback) continue;

        status = crc_callback(&chain->items[i].data);
        break;
    }
    return status;
}

static NfcHistoryCrcStatus
    nfc_history_get_crc_status(const NfcHistory* history, const NfcMode mode) {
    NfcHistoryCrcStatus status = NfcHistoryCrcNotSet;
    for(size_t i = 0; i < history->base.chain_count; i++) {
        status = nfc_history_find_crc_from_chain(&history->chains[i], mode);
        if(status != NfcHistoryCrcNotSet) break;
    }
    return status;
}

static void nfc_formatter_format(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    FuriString* output) {
    furi_assert(instance->protocol != NfcProtocolInvalid);

    instance->crc_from_history = nfc_history_get_crc_status(transaction->history, instance->mode);

    nfc_transaction_format(instance, transaction, output);
}

static bool nfc_logger_open_log(
    Stream* stream,
    FuriString* file_path_no_ext,
    const char* extension,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    FuriString* file_path = furi_string_alloc_set(file_path_no_ext);
    furi_string_cat_str(file_path, extension);

    bool result =
        file_stream_open(stream, furi_string_get_cstr(file_path), access_mode, open_mode);

    furi_string_free(file_path);
    return result;
}

static bool nfc_logger_read_trace(Stream* stream, NfcTrace* trace) {
    bool result = false;
    do {
        size_t read_bytes = stream_read(stream, (uint8_t*)trace, sizeof(NfcTrace));
        if(read_bytes != sizeof(NfcTrace)) {
            FURI_LOG_E(TAG, "Wrong trace size");
            break;
        }

        if(trace->mode >= NfcModeNum) {
            FURI_LOG_E(TAG, "Invalid mode %02X in trace", trace->mode);
            break;
        }

        if(trace->protocol >= NfcProtocolNum) {
            FURI_LOG_E(TAG, "Invalid protocol %02X in trace", trace->protocol);
            break;
        }

        result = true;
    } while(false);

    return result;
}

static void nfc_logger_convert_bin_to_text(
    Storage* storage,
    FuriString* file_path,
    const NfcLoggerFormatterConfig* config) {
    Stream* stream_bin = file_stream_alloc(storage);
    Stream* stream_txt = file_stream_alloc(storage);

    do {
        NfcTrace trace;
        if(!nfc_logger_open_log(stream_bin, file_path, ".bin", FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Unable to open log file: %s.bin", furi_string_get_cstr(file_path));
            break;
        }

        if(!nfc_logger_open_log(stream_txt, file_path, ".txt", FSAM_WRITE, FSOM_CREATE_NEW)) {
            FURI_LOG_E(TAG, "Unable to create log file: %s.txt", furi_string_get_cstr(file_path));
            break;
        }

        FURI_LOG_D(TAG, "Processing log file: %s", furi_string_get_cstr(file_path));
        if(!nfc_logger_read_trace(stream_bin, &trace)) break;

        NfcFormatter* formatter = nfc_formatter_alloc(config);

        FuriString* str = furi_string_alloc();
        nfc_format_trace(formatter, file_path, &trace, str);
        stream_write_string(stream_txt, str);

        furi_string_reset(str);

        nfc_format_table_header(formatter, str);
        stream_write_string(stream_txt, str);

        NfcTransaction* transaction;
        size_t transaction_cnt = 0;
        while(nfc_transaction_read(stream_bin, &transaction)) {
            furi_string_reset(str);
            nfc_formatter_format(formatter, transaction, str);
            stream_write_string(stream_txt, str);

            nfc_transaction_free(transaction);
            transaction_cnt++;
        }

        furi_string_free(str);
        nfc_formatter_free(formatter);

        FURI_LOG_I(
            TAG,
            "Log file %s processed, %lu transactions saved",
            furi_string_get_cstr(file_path),
            (uint32_t)transaction_cnt);
    } while(false);

    stream_free(stream_bin);
    stream_free(stream_txt);
}

void nfc_logger_formatter_run(Nfc* nfc, const NfcLoggerFormatterConfig* config) {
    furi_assert(nfc);
    furi_assert(config);

    NfcLogger* logger = nfc_get_logger(nfc);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_path = furi_string_alloc();

    while(nfc_logger_raw_log_file_present(logger, file_path)) {
        nfc_logger_convert_bin_to_text(storage, file_path, config);
    }

    furi_string_free(file_path);
    furi_record_close(RECORD_STORAGE);
}

/* Actual implementation of app<>plugin interface */
static const NfcLoggerFormatterPlugin formatter_plugin = {
    .format = nfc_logger_formatter_run,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor formatter_plugin_descriptor = {
    .appid = NFC_LOGGER_FORMATTER_PLUGIN_APP_ID,
    .ep_api_version = NFC_LOGGER_FORMATTER_PLUGIN_API_VERSION,
    .entry_point = &formatter_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* formatter_plugin_ep(void) {
    return &formatter_plugin_descriptor;
}
