#pragma once

#include <cli/cli.h>
#include "../../../../types/plugin_state.h"

void totp_cli_handle_delete_command(PluginState* plugin_state, FuriString* args, Cli* cli);