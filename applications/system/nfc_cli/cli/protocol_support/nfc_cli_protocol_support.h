#pragma once

#include "../nfc_cli_i.h"

void nfc_cli_protocol_support_print_usage();

bool nfc_cli_protocol_support_cmd_process(NfcCli* nfc_cli, const FuriString* args);
