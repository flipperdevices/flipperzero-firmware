#include "iso14443_3a.h"

#include "../nfc_cli_protocol_support_common.h"

static void iso14443_3a_cmd_handler_poll(Cli* cli, FuriString* args) {
    // nfc_cli_protocol_support_common_poll_handler(NfcProtocolIso14443_3a, )
}

static void iso14443_3a_cmd_handler_start_poller(Cli* cli, FuriString* args) {
    UNUSED(cli);
    UNUSED(args);

    printf("Hello iso14443-3a start_poller\r\n");
}

const NfcCliProtocolSupportBase nfc_cli_protocol_support_base_iso14443_3a = {
    .protocol = NfcProtocolIso14443_3a,
    .cmd_name = "iso14443-3a",
    .cmd_handler =
        {
            iso14443_3a_cmd_handler_poll,
            iso14443_3a_cmd_handler_start_poller,
        },
};
