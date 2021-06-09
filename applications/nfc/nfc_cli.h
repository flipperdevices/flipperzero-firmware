#pragma once

#include <cli/cli.h>

void nfc_cli_init();

void nfc_cli_detect(Cli* cli, string_t args, void* context);