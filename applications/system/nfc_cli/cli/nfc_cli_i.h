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

size_t nfc_cli_read(NfcCli* instance, uint8_t* data, size_t data_size);

void nfc_cli_scanner(NfcCli* instance, FuriString* args);

void nfc_cli_field(NfcCli* instance, FuriString* args);
