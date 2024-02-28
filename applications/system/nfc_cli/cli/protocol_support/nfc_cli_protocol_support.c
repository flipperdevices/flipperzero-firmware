#include "nfc_cli_protocol_support.h"

#include "nfc_cli_protocol_support_defs.h"

#include <furi/furi.h>
#include <lib/toolbox/args.h>
#include <lib/toolbox/hex.h>

void nfc_cli_protocol_support_print_usage() {
    for(size_t i = 0; i < NfcProtocolNum; i++) {
        if(nfc_cli_protocol_support[i]) {
            printf(
                "\t%s\t\t - %s protocol commands\r\n",
                nfc_cli_protocol_support[i]->cmd_name,
                nfc_cli_protocol_support[i]->cmd_name);
        }
    }
}

static void
    nfc_cli_protocol_support_print_command_usage(NfcProtocol protocol, const char* input_cmd) {
    if(input_cmd != NULL) {
        printf("Incorrect command: %s\r\n", input_cmd);
    }

    if(nfc_cli_protocol_support[protocol] != NULL) {
        for(size_t i = 0; i < NfcCliProtocolSupportCommandNum; i++) {
            if(nfc_cli_protocol_support[protocol]->cmd_handler[i] != NULL) {
                printf("\t%s\r\n", nfc_cli_protocol_support_get_command_name(i));
            }
        }
    }
}

static void
    nfc_cli_protocol_support_handle_cmd(NfcProtocol protocol, NfcCli* nfc_cli, FuriString* args) {
    FuriString* tmp_str = furi_string_alloc();

    do {
        bool command_processed = false;

        if(!args_read_string_and_trim(args, tmp_str)) {
            nfc_cli_protocol_support_print_command_usage(protocol, NULL);
            break;
        }

        for(size_t i = 0; i < NfcCliProtocolSupportCommandNum; i++) {
            if(furi_string_cmp_str(tmp_str, nfc_cli_protocol_support_get_command_name(i)) == 0) {
                if(nfc_cli_protocol_support[protocol]->cmd_handler[i] != NULL) {
                    nfc_cli_protocol_support[protocol]->cmd_handler[i](nfc_cli, args);
                    command_processed = true;
                }
            }
        }
        if(command_processed) break;

        nfc_cli_protocol_support_print_command_usage(protocol, furi_string_get_cstr(args));

    } while(false);

    furi_string_free(tmp_str);
}

bool nfc_cli_protocol_support_cmd_process(NfcCli* nfc_cli, const FuriString* args) {
    furi_assert(nfc_cli);
    furi_assert(args);

    bool processed = false;

    FuriString* args_copy = furi_string_alloc_set(args);
    FuriString* protocol_str = furi_string_alloc();

    do {
        if(!args_read_string_and_trim(args_copy, protocol_str)) {
            break;
        }

        for(size_t i = 0; i < NfcProtocolNum; i++) {
            if(nfc_cli_protocol_support[i] != NULL) {
                if(furi_string_cmp_str(protocol_str, nfc_cli_protocol_support[i]->cmd_name) == 0) {
                    nfc_cli_protocol_support_handle_cmd(i, nfc_cli, args_copy);
                    processed = true;
                    break;
                }
            }
        }
    } while(false);

    furi_string_free(protocol_str);
    furi_string_free(args_copy);

    return processed;
}
