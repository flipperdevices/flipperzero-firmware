#pragma once

#include <toolbox/cli/cli_command.h>
#include <toolbox/cli/cli_registry.h>

#define RECORD_CLI_MASTER "cli_master_registry"
#define CLI_MASTER_APPID  "cli_master"

void cli_master_commands_init(CliRegistry* registry);
