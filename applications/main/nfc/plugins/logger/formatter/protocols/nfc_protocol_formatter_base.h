#pragma once

#include "../nfc_formatter_context.h"

#include <furi.h>
#include <nfc/helpers/logger/transaction/nfc_transaction.h>
#include <nfc/helpers/logger/transaction/nfc_packet_data_type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*NfcTransactionDecryptCallback)(
    const NfcPacket* input,
    NfcHistoryData* item,
    NfcPacket* output);

typedef void (*NfcHistoryItemDataFormatCallback)(
    const NfcPacket* packet,
    const NfcHistoryData* data,
    FuriString* output);

//typedef void (*NfcFormatterCallbackFormatPacket)(const NfcPacket* pack, FuriString* output);

typedef void (*NfcTransactionFormatCallback)(
    const NfcFormatter* instance,
    const NfcTransaction* transaction,
    FuriString* output);

typedef NfcHistoryCrcStatus (*NfcHistoryGetCrcStatusCallback)(const NfcHistoryData* data);

typedef struct {
    NfcHistoryItemDataFormatCallback format_history;
    NfcHistoryItemDataFormatCallback format_history_simplified;
    NfcTransactionDecryptCallback decrypt;
    NfcHistoryGetCrcStatusCallback get_crc_status;
} NfcProtocolFormatterBase;

#ifdef __cplusplus
}
#endif
