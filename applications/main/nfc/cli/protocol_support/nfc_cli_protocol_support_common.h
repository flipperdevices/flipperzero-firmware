#pragma once

#include <nfc/nfc_poller.h>
#include <nfc/protocols/nfc_protocol.h>

typedef void(NfcCliProtocolSupportCommonCallback)(NfcGenericEventEx event);

void nfc_cli_protocol_support_common_poll_handler(
    NfcProtocol protocol,
    NfcCliProtocolSupportCommonCallback callback);
