#include "nfc_cli_command_mfu.h"
#include "../nfc_cli_command_processor.h"

typedef struct {
    Nfc* nfc;
    NfcProtocol protocol;
} NfcMfuInfoCtx;

static NfcCliActionContext* nfc_cli_info_alloc_ctx(Nfc* nfc) {
    NfcMfuInfoCtx* instance = malloc(sizeof(NfcMfuInfoCtx));
    instance->protocol = NfcProtocolMfUltralight;
    instance->nfc = nfc;
    return instance;
}

const NfcCliActionDescriptor info_action = {
    .name = "info",
    .description = "Get basic information about the card",
    .alloc = nfc_cli_info_alloc_ctx,
    //.free = nfc_cli_raw_free_ctx,
    //.execute = nfc_cli_raw_command,
    .key_count = 0,
    .keys = NULL,
};

const NfcCliKeyDescriptor rdbl_action_keys[] = {
    {
        .short_name = "b",
        .long_name = "block",
        .features = {.required = true, .parameter = true},
        .description = "desired block number",
        //.parse = parse_block_key,
    },
    {
        .short_name = "k",
        .long_name = "key",
        .features = {.parameter = true},
        //.parse = parse_key_
    },
};

//mfu rdbl -b 0
//mfu rdbl -block 0
//mfu rdbl -b 0 -k DEADBEAF
const NfcCliActionDescriptor rdbl_action = {
    .name = "rdbl",
    .description = "Read block from ultralight card",
    //.alloc = nfc_cli_raw_alloc_ctx,
    //.free = nfc_cli_raw_free_ctx,
    //.execute = nfc_cli_raw_command,
    .key_count = COUNT_OF(rdbl_action_keys),
    .keys = rdbl_action_keys,
};

const NfcCliActionDescriptor* mfu_actions[] = {
    &rdbl_action,
    &info_action,
};

//Command descriptor
ADD_NFC_CLI_COMMAND(mfu, "Mifare Ultralight specific commands", mfu_actions);
/* const NfcCliCommandDescriptor mfu_cmd = {
    .name = "mfu",
    .action_count = COUNT_OF(mfu_actions),
    .actions = mfu_actions,
}; */
