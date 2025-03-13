#include "nfc_cli_command_raw.h"
#include "../nfc_cli_command_processor.h"

///TODO: use this in parsing of arg_protocol
/* const char* protocol_key_valid_values[] =
    {"14a", "14b", "15", "felica", "iso14a", "iso14b", "iso15"}; */

const NfcCliKeyDescriptor raw_action_keys[] = {
    {
        .long_name = NULL,
        .short_name = "k",
        .description = "keep signal field ON after receive",
        /* .parse_arg = nfc_key_arg_bool_parse, */
    },
    {
        .long_name = NULL,
        .short_name = "c",
        .description = "calculate and append CRC",
        /* .parse_arg = nfc_key_arg_bool_parse */
    },
    {
        .long_name = NULL,
        .short_name = "s",
        .description = "Select on FieldOn",
        /* .parse_arg = nfc_key_arg_bool_parse */
    },
    /*     {
        .mandatory = true,
        .features = {.mandatory = true, .parameter = true, .ommitable = true},
        .long_name = "protocol",
        .short_name = "p",
        .description = "Desired protocol",
        //.parse_arg = raw_parse_protocol

        ///TODO: Will try to get rid of this
        // .valid_values_count = COUNT_OF(protocol_key_valid_values),
        // .valid_values = protocol_key_valid_values,

    },
    {
        .mandatory = true,
        .features = {.mandatory = true, .parameter = true, .ommitable = true},
        .long_name = "data",
        .short_name = "d",
        .description = "Required data",
        //.parse_arg = raw_parse_data
    }, */
};

const NfcCliArgumentDescriptor raw_arguments[] = {
    {
        .name = "protocol",
        /* .parse = raw_parse_protocol */
    },
    {
        .name = "data",
        /* .parse = raw_parse_data */
    },
};

const NfcCliActionDescriptor raw_action = {
    .name = "raw",
    //.alloc = nfc_cli_raw_alloc_ctx,
    //.free = nfc_cli_raw_free_ctx,
    //.execute = nfc_cli_raw_command,
    .argument_count = COUNT_OF(raw_arguments),
    .arguments = raw_arguments,
    .key_count = COUNT_OF(raw_action_keys),
    .keys = raw_action_keys,
};

const NfcCliActionDescriptor* raw_actions_collection[] = {&raw_action};
//Command descriptor

//ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

//static void nfc_cli_command_raw_callback(PipeSide* pipe, FuriString* args, void* context);
ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

/* const NfcCliCommandDescriptor raw_cmd = {
    .name = "raw",
    .action_count = COUNT_OF(raw_actions_collection),
    .actions = raw_actions_collection,
    .callback = nfc_cli_command_raw_callback,
} */
;
/* 
static void nfc_cli_command_raw_callback(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    nfc_cli_command_process(&raw_cmd, args, context);
} */

//ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

//Command usage: raw <protocol> [keys] <data>
//Command examples:
//raw iso14a -sc 3000
//raw iso14a 3000
//raw iso14a 3000 -sc
