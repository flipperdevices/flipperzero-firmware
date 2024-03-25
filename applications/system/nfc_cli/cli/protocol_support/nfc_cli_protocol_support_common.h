#pragma once

#include <nfc/nfc_poller.h>
#include "../nfc_cli_i.h"

typedef enum {
    NfcCliPollerErrorNone,
    NfcCliPollerErrorTimeout,
    NfcCliPollerErrorInternal,
} NfcCliPollerError;

typedef struct {
    BitBuffer* tx_data;
    bool append_crc;
    uint32_t timeout;
} NfcCliPollCmdData;

typedef enum {
    NfcCliProtocolRequestTypeActivate,
    NfcCliProtocolRequestTypeFrameExchange,
} NfcCliProtocolRequestType;

typedef struct {
    const NfcCliPollCmdData* tx_data;
    BitBuffer* rx_data;
} NfcCliProtocolRequestFrameExchangeData;

typedef struct {
    Nfc* nfc;
    NfcGenericInstance* poller;
    NfcCliPollerError error;
    union {
        NfcCliProtocolRequestFrameExchangeData frame_exchange;
        FuriString* activation_info;
    };
} NfcCliProtocolRequestData;

typedef struct {
    NfcCliProtocolRequestType type;
    NfcCliProtocolRequestData data;
} NfcCliProtocolRequest;

typedef void (*NfcCliProtocolSupportCommonCallback)(NfcCliProtocolRequest* request);

void nfc_cli_protocol_support_common_poll_handler(
    NfcProtocol protocol,
    NfcCli* nfc_cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback);

void nfc_cli_protocol_support_common_start_poller_handler(
    NfcProtocol protocol,
    NfcCli* nfc_cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback);
