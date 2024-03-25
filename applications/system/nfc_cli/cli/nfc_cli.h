#pragma once

#include <cli/cli.h>

typedef struct NfcCli NfcCli;

NfcCli* nfc_cli_alloc(Cli* cli);

void nfc_cli_free(NfcCli* instance);

bool nfc_cli_is_command_in_progress(NfcCli* instance);

void nfc_cli_abort_command(NfcCli* instance);
