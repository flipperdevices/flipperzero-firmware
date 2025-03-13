#include "nfc_cli_command_emulate.h"
#include "../nfc_cli_command_processor.h"

const NfcCliKeyDescriptor emulate_keys[] = {
    {
        .features = {.mandatory = true, .parameter = true, .ommitable = true},
        .long_name = "file",
        .short_name = "f",
        .description = "path to .nfc file",
        //.parse = emulate_parse_filename_key
    },
};

const NfcCliActionDescriptor emulate_action = {
    .name = "emulate",
    //.alloc = nfc_cli_raw_alloc_ctx,
    //.free = nfc_cli_raw_free_ctx,
    //.execute = nfc_cli_raw_command,
    .argument_count = 0,
    .arguments = 0,
    .key_count = COUNT_OF(emulate_keys),
    .keys = emulate_keys,
};

const NfcCliActionDescriptor* emulate_actions_collection[] = {&emulate_action};

//Command descriptor
ADD_NFC_CLI_COMMAND(emulate, emulate_actions_collection);

/* const NfcCliCommandDescriptor emulate_cmd = {
    .name = "emulate",
    .action_count = 1,
    .actions = emulate_actions_collection,
};
 */
//Command usage: emulate [-f <file>]
//Command examples:
//emulate -f ext/nfc/test.nfc
//emulate ext/nfc/test.nfc
