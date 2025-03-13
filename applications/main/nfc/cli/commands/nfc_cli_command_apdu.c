#include "nfc_cli_command_apdu.h"
#include "../nfc_cli_command_processor.h"

const NfcCliArgumentDescriptor apdu_arguments[] = {
    {
        .name = "protocol",
        /* .parse = apdu_parse_protocol */
    },
    {
        .name = "data",
        /* .parse = apdu_parse_data */
    },
};

const NfcCliActionDescriptor apdu_action = {
    .name = "apdu",
    //.alloc = nfc_cli_apdu_alloc_ctx,
    //.free = nfc_cli_apdu_free_ctx,
    //.execute = nfc_cli_apdu_command,
    .argument_count = COUNT_OF(apdu_arguments),
    .arguments = apdu_arguments,
    .key_count = 0,
    .keys = NULL,
};

const NfcCliActionDescriptor* apdu_actions_collection[] = {&apdu_action};

//Command descriptor
ADD_NFC_CLI_COMMAND(apdu, apdu_actions_collection);
/* const NfcCliCommandDescriptor apdu_cmd = {
    .name = "apdu",
    .action_count = 1,
    .actions = apdu_actions_collection,
}; */

//Command usage: apdu <protocol> <data>
//Command examples:
//apdu 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 4a 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 4b 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 15 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102