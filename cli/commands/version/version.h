#pragma once

#include "../../../config/app/config.h"

#define TOTP_CLI_COMMAND_VERSION "version"

void totp_cli_command_version_handle();
#ifdef TOTP_CLI_RICH_HELP_ENABLED
void totp_cli_command_version_docopt_commands();
void totp_cli_command_version_docopt_usage();
#endif