#pragma once

/* 
 * Compatibility header for ease of porting existing apps.
 *  In short: 
 *   cli_add_command() is replaced with cli_registry_add_command(),
 *   Cli* - with CliRegistry*
*/

#include <toolbox/cli/cli_registry.h>

#define RECORD_CLI "cli"
