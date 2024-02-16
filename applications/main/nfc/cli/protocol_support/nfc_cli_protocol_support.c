#include "nfc_cli_protocol_support.h"

#include "nfc_cli_protocol_support_defs.h"

#include <furi/furi.h>

void nfc_cli_protocol_support_print_usage() {
    for(size_t i = 0; i < NfcProtocolNum; i++) {
        if(nfc_cli_protocol_support[i] != NULL) {
            printf("\t%s\t - poll commands\r\n", nfc_cli_protocol_support[i]->cmd_name);
        }
    }
}

bool nfc_cli_protocol_support_cmd_process(Cli* cli, FuriString* args) {
    UNUSED(cli);
    UNUSED(args);

    bool processed = false;
    for(size_t i = 0; i < NfcProtocolNum; i++) {
        if(nfc_cli_protocol_support[i] != NULL) {
            if(furi_string_cmp_str(args, nfc_cli_protocol_support[i]->cmd_name) == 0) {
                printf("Processed\r\n");
                processed = true;
                break;
            }
        }
    }

    return processed;
}
