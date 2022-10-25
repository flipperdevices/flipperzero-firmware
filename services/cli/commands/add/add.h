#pragma once

#include <cli/cli.h>
#include "../../../../types/plugin_state.h"

#define TOTP_CLI_COMMAND_ADD "add"

void totp_cli_command_add_handle(PluginState* plugin_state, FuriString* args, Cli* cli);
void totp_cli_command_add_print_help();