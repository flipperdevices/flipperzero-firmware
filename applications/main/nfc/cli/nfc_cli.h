#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <cli/shell/cli_shell.h>
#include <cli/cli.h>
#include <cli/cli_vcp.h>
#include <cli/cli_commands.h>
#include <args.h>
#include <hex.h>
#include <toolbox/pipe.h>
#include <nfc/nfc.h>

#include "nfc_cli_command_base.h"
#include "nfc_cli_command_processor.h"

typedef struct {
    Nfc* nfc;
    Cli* nfc_cli;
    FuriThread* shell;
    NfcCliProcessorContext* processor_context;
} NfcCliContext;
