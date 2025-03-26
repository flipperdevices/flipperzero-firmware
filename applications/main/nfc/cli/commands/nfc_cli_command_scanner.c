
#include "nfc_cli_command_scanner.h"
#include "helpers/nfc_cli_scanner.h"

static NfcCliActionContext* nfc_cli_command_scanner_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);

    NfcCliScanner* instance = nfc_cli_scanner_alloc(nfc);
    return instance;
}

static void nfc_cli_command_scanner_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    NfcCliScanner* instance = ctx;
    nfc_cli_scanner_free(instance);
}

static void nfc_cli_command_scanner_execute(PipeSide* pipe, void* context) {
    NfcCliScanner* instance = context;

    printf("Press Ctrl+C to abort\r\n\n");
    nfc_cli_scanner_begin_scan(instance);
    while(!cli_is_pipe_broken_or_is_etx_next_char(pipe) &&
          !nfc_cli_scanner_wait_scan(instance, 50))
        ;
    nfc_cli_scanner_end_scan(instance);
    nfc_cli_scanner_list_detected_protocols(instance);
}

const NfcCliActionDescriptor scanner_action = {
    .name = "scanner",
    .description = "Detect tag type",
    .key_count = 0,
    .keys = NULL,
    .execute = nfc_cli_command_scanner_execute,
    .alloc = nfc_cli_command_scanner_alloc_ctx,
    .free = nfc_cli_command_scanner_free_ctx,
};

const NfcCliActionDescriptor* scanner_actions_collection[] = {&scanner_action};

ADD_NFC_CLI_COMMAND(scanner, "", scanner_actions_collection);
