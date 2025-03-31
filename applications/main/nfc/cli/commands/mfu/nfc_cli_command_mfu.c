#include "nfc_cli_command_mfu.h"
#include "nfc_cli_action_info.h"

#define TAG "MFU"

static void nfc_cli_mfu_not_implemented_dummy_execute(PipeSide* pipe, NfcCliActionContext* ctx) {
    UNUSED(pipe);
    UNUSED(ctx);
    printf("Not implemented");
}

//mfu info
const NfcCliActionDescriptor info_action = {
    .name = "info",
    .description = "Get basic information about the card",
    .alloc = nfc_cli_info_alloc_ctx,
    .free = nfc_cli_info_free_ctx,
    .execute = nfc_cli_mfu_info_execute,
    .key_count = 0,
    .keys = NULL,
};

const NfcCliKeyDescriptor rdbl_action_keys[] = {
    {
        .short_name = "b",
        .long_name = "block",
        .features = {.required = true, .parameter = true},
        .description = "desired block number",
    },
    {
        .short_name = "k",
        .long_name = "key",
        .features = {.parameter = true},
    },
};

//mfu rdbl -b 0
//mfu rdbl -block 0
//mfu rdbl -b 0 -k DEADBEAF
const NfcCliActionDescriptor rdbl_action = {
    .name = "rdbl",
    .description = "Read block from ultralight card",
    .alloc = NULL,
    .free = NULL,
    .execute = nfc_cli_mfu_not_implemented_dummy_execute,
    .key_count = COUNT_OF(rdbl_action_keys),
    .keys = rdbl_action_keys,
};

const NfcCliActionDescriptor* mfu_actions[] = {
    &rdbl_action,
    &info_action,
};

//Command descriptor
ADD_NFC_CLI_COMMAND(mfu, "Mifare Ultralight specific commands", mfu_actions);
