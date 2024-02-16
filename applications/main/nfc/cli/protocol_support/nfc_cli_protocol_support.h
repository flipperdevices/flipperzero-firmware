#pragma once

#include <cli/cli.h>

void nfc_cli_protocol_support_print_usage();

bool nfc_cli_protocol_support_cmd_process(Cli* cli, FuriString* args);
