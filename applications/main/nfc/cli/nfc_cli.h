#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <cli/shell/cli_shell.h>
#include <toolbox/cli/shell/cli_shell.h>
#include <cli/cli_vcp.h>
#include <toolbox/cli/cli_command.h>
#include <args.h>
#include <hex.h>
#include <toolbox/pipe.h>
#include <nfc/nfc.h>

#include "nfc_cli_command_base.h"
#include "nfc_cli_command_processor.h"

typedef struct {
    Nfc* nfc;
    CliRegistry* registry;
    CliShell* shell;
    NfcCliProcessorContext* processor_context;
} NfcCliContext;
