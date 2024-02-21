#include "nfc_cli_protocol_support_defs.h"

#include <stddef.h>

#include "iso14443_3a/iso14443_3a.h"
#include "iso14443_3b/iso14443_3b.h"

static const char* nfc_cli_protocol_support_command_name[NfcCliProtocolSupportCommandNum] = {
    [NfcCliProtocolSupportCommandPoll] = "poll",
    [NfcCliProtocolSupportCommandStartPoller] = "start_poller",
};

const char* nfc_cli_protocol_support_get_command_name(NfcCliProtocolSupportCommand cmd) {
    furi_assert(cmd < NfcCliProtocolSupportCommandNum);

    return nfc_cli_protocol_support_command_name[cmd];
}

const NfcCliProtocolSupportBase* nfc_cli_protocol_support[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_cli_protocol_support_base_iso14443_3a,
    [NfcProtocolIso14443_3b] = &nfc_cli_protocol_support_base_iso14443_3b,
    [NfcProtocolIso14443_4a] = NULL,
    [NfcProtocolIso14443_4b] = NULL,
    [NfcProtocolIso15693_3] = NULL,
    [NfcProtocolFelica] = NULL,
    [NfcProtocolMfUltralight] = NULL,
    [NfcProtocolMfClassic] = NULL,
    [NfcProtocolMfDesfire] = NULL,
    [NfcProtocolSlix] = NULL,
    [NfcProtocolSt25tb] = NULL,
};
