#pragma once

#include "../../../../types/plugin_state.h"

#define TOTP_CLI_COMMAND_LIST "list"

void totp_cli_command_list_handle(PluginState* plugin_state);
void totp_cli_command_list_print_help();