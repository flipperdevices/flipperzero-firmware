#include "nfc_cli_i.h"

#include <furi.h>

static void nfc_cli_print_usage() {
    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    printf("\tdetect\t\t - detect NFC tag\r\n");
    printf("\tfield\t\t - turn field on\r\n");
    // nfc_cli_protocol_support_print_usage();
}

bool nfc_cli_abort_received(NfcCli* instance) {
    furi_assert(instance);

    return cli_cmd_interrupt_received(instance->cli) ||
           (instance->command_status == NfcCliCommandStatusAbortRequested);
}

static void nfc_cli(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);
    UNUSED(cli);

    NfcCli* instance = context;
    instance->command_status = NfcCliCommandStatusStarted;

    FuriString* cmd = furi_string_alloc();
    // Remove this shit
    FuriString* tmp_str = furi_string_alloc_set(args);

    do {
        if(!args_read_string_and_trim(args, cmd)) {
            nfc_cli_print_usage();
            break;
        }

        if(furi_string_cmp_str(cmd, "detect") == 0) {
            nfc_cli_scanner(instance, args);
            break;
        } else if(furi_string_cmp_str(cmd, "field") == 0) {
            nfc_cli_field(instance, args);
            break;
        }
        nfc_cli_print_usage();
    } while(false);

    furi_string_free(tmp_str);
    furi_string_free(cmd);

    instance->command_status = NfcCliCommandStatusIdle;
}

NfcCli* nfc_cli_alloc(Cli* cli) {
    furi_assert(cli);

    NfcCli* instance = malloc(sizeof(NfcCli));
    instance->nfc = nfc_alloc();
    instance->cli = cli;
    instance->command_status = NfcCliCommandStatusIdle;
    cli_add_command(instance->cli, "nfc", CliCommandFlagParallelSafe, nfc_cli, instance);

    return instance;
}

void nfc_cli_free(NfcCli* instance) {
    furi_assert(instance);

    nfc_free(instance->nfc);
    cli_delete_command(instance->cli, "nfc");
}

bool nfc_cli_is_command_in_progress(NfcCli* instance) {
    furi_assert(instance);

    return instance->command_status != NfcCliCommandStatusIdle;
}

void nfc_cli_abort_command(NfcCli* instance) {
    furi_assert(instance);

    instance->command_status = NfcCliCommandStatusAbortRequested;

    while(instance->command_status != NfcCliCommandStatusIdle) {
        furi_delay_ms(50);
    }
}
