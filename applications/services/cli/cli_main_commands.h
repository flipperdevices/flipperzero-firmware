#pragma once

#include "cli.h"
#include <cli/cli_command.h>
#include <cli/cli_registry.h>

#define CLI_APPID "cli"

void cli_main_commands_init(CliRegistry* registry);
