#pragma once
#include "nfc_cli.h"

void nfc_cli_command_process(const NfcCliCommandDescriptor* cmd, FuriString* args, void* context);
