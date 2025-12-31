#include "nfc_history_formatter.h"
#include "../protocols/nfc_hal/nfc_hal_formatter.h"
#include "../protocols/nfc_protocol_formatters.h"
#include <nfc/helpers/logger/history/nfc_history_chain_data_type.h>

#define TAG "NfcHistoryFormatter"

static bool
    nfc_history_layer_filter_skip(const NfcFormatter* instance, const NfcProtocol item_protocol) {
    const NfcLoggerHistoryLayerFilter filter = instance->config->history_filter;

    return (
        (filter == NfcLoggerHistoryLayerFilterTopProtocol ||
         filter == NfcLoggerHistoryLayerFilterTopProtocolSimplified) &&
        item_protocol != instance->protocol);
}

static void nfc_history_format_hal_item(const FuriHalNfcEvent nfc_event, FuriString* output) {
    FuriString* layer_parsed_str = furi_string_alloc();
    nfc_hal_data_format_hal_event_type(nfc_event, layer_parsed_str);
    furi_string_printf(output, "L0(%s)", furi_string_get_cstr(layer_parsed_str));
    furi_string_free(layer_parsed_str);
}

static NfcHistoryItemDataFormatCallback nfc_history_get_item_format_callback(
    const NfcProtocol protocol,
    const NfcMode mode,
    const NfcLoggerHistoryLayerFilter filter) {
    NfcHistoryItemDataFormatCallback callback = NULL;
    do {
        const NfcProtocolFormatterBase* protocol_formatter =
            nfc_protocol_formatter_get(protocol, mode);
        if(!protocol_formatter) break;

        if(filter == NfcLoggerHistoryLayerFilterAll ||
           filter == NfcLoggerHistoryLayerFilterTopProtocol) {
            callback = protocol_formatter->format_history;
        } else if(filter == NfcLoggerHistoryLayerFilterTopProtocolSimplified) {
            callback = protocol_formatter->format_history_simplified;
            if(callback == NULL) {
                FURI_LOG_W(TAG, "Simplified formatting not implemented for protocol %d", protocol);
            }
        }
    } while(false);
    return callback;
}

static void nfc_history_format_item(
    const NfcFormatter* instance,
    const NfcPacket* request,
    const NfcHistoryItem* item,
    FuriString* output) {
    do {
        NfcHistoryItemDataFormatCallback callback = nfc_history_get_item_format_callback(
            item->base.protocol, instance->mode, instance->config->history_filter);

        if(callback == NULL) {
            furi_string_printf(output, "Not implemented");
            break;
        }

        callback(request, &item->data, output);
    } while(false);
}

static void nfc_history_format_layer(
    const size_t layer_num,
    const NfcLoggerHistoryLayerFilter filter,
    const FuriString* formatted_item,
    FuriString* annotation) {
    if(filter == NfcLoggerHistoryLayerFilterAll ||
       filter == NfcLoggerHistoryLayerFilterTopProtocol) {
        const char* format = (filter == NfcLoggerHistoryLayerFilterTopProtocol) ? "L%d(%s)" :
                                                                                  "->L%d(%s)";
        furi_string_cat_printf(
            annotation, format, layer_num, furi_string_get_cstr(formatted_item));
    } else if(filter == NfcLoggerHistoryLayerFilterTopProtocolSimplified) {
        furi_string_printf(annotation, "%s", furi_string_get_cstr(formatted_item));
    }
}

void nfc_histroy_format_annotation(
    const NfcFormatter* instance,
    const NfcPacket* request,
    const NfcHistory* history,
    const FuriHalNfcEvent nfc_event,
    FuriString* annotation) {
    furi_assert(instance);
    furi_assert(history);
    furi_assert(annotation);

    const NfcLoggerHistoryLayerFilter filter = instance->config->history_filter;

    if(filter == NfcLoggerHistoryLayerFilterAll) {
        nfc_history_format_hal_item(nfc_event, annotation);
    }

    FuriString* layer_parsed_str = furi_string_alloc();
    for(size_t i = 0; i < history->base.chain_count; i++) {
        for(size_t j = 0; j < history->chains[i].length; j++) {
            furi_string_reset(layer_parsed_str);
            const NfcHistoryItem* item = &(history->chains[i].items[j]);

            if(nfc_history_layer_filter_skip(instance, item->base.protocol)) continue;
            nfc_history_format_item(instance, request, item, layer_parsed_str);
            nfc_history_format_layer(j + 1, filter, layer_parsed_str, annotation);
        }
    }

    furi_string_free(layer_parsed_str);
}

const NfcHistoryItem* nfc_history_get_item(const NfcHistory* history, const NfcProtocol protocol) {
    for(size_t i = 0; i < history->base.chain_count; i++) {
        for(size_t j = 0; j < history->chains[i].length; j++) {
            const NfcHistoryItem* item = &(history->chains[i].items[j]);
            if(item->base.protocol != protocol) continue;
            return item;
        }
    }

    return NULL;
}
