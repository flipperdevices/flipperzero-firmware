#pragma once

#include "nfc_cli.h"

#include <nfc/nfc.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>
#include <lib/toolbox/hex.h>

typedef enum {
    NfcCliCommandStatusIdle,
    NfcCliCommandStatusStarted,
    NfcCliCommandStatusAbortRequested,
} NfcCliCommandStatus;

struct NfcCli {
    Nfc* nfc;
    Cli* cli;
    NfcCliCommandStatus command_status;
};

bool nfc_cli_abort_received(NfcCli* instance);

void nfc_cli_scanner(NfcCli* instance, FuriString* args);

void nfc_cli_field(NfcCli* instance, FuriString* args);
