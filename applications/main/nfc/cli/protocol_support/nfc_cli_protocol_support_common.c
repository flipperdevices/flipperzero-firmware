#include "nfc_cli_protocol_support_common_i.h"

#include <furi/furi.h>

void nfc_cli_protocol_support_common_poll_handler(
    NfcProtocol protocol,
    NfcCliProtocolSupportCommonCallback callback) {
    furi_assert(protocol < NfcProtocolNum);
    UNUSED(callback);
}
