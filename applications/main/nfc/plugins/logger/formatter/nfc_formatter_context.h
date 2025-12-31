#pragma once

#include "helpers/table.h"
#include "helpers/nfc_transaction_string.h"
#include "../nfc_logger_config_data_type.h"

#include <nfc_mode.h>
#include <nfc/protocols/nfc_protocol.h>
#include <nfc/helpers/logger/transaction/nfc_packet_data_type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcHistoryCrcNotSet,
    NfcHistoryCrcOk,
    NfcHistoryCrcBad
} NfcHistoryCrcStatus;

typedef struct {
    NfcPacket* request;
    NfcPacket* response;
} NfcTransactionDecryptedData;

typedef void NfcTransactionDecryptContext;

typedef struct {
    NfcProtocol protocol;
    NfcMode mode;
    uint8_t table_columns_cnt;
    NfcHistoryCrcStatus crc_from_history;
    NfcTransactionDecryptContext* decrypt_context;
    NfcTransactionDecryptedData* decrypted_transaction;
    const NfcLoggerFormatterConfig* config;
    Table* table;
} NfcFormatter;

#ifdef __cplusplus
}
#endif
