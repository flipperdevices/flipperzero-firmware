#pragma once

#include <cli/cli.h>
#include "../../../../types/plugin_state.h"

#define TOTP_CLI_COMMAND_DELETE "delete"

void totp_cli_command_delete_handle(PluginState* plugin_state, FuriString* args, Cli* cli);
void totp_cli_command_delete_print_help();