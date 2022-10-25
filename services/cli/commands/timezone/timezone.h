#pragma once

#include <cli/cli.h>
#include "../../../../types/plugin_state.h"

#define TOTP_CLI_COMMAND_TIMEZONE "timezone"

void totp_cli_command_timezone_handle(PluginState* plugin_state, FuriString* args, Cli* cli);
void totp_cli_command_timezone_print_help();