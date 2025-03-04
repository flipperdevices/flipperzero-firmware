#pragma once

#include "../cli.h"
#include <furi.h>
#include <toolbox/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_SHELL_STACK_SIZE (2 * 1024U)

FuriThread* cli_shell_start(PipeSide* pipe);
FuriThread* cli_shell_start_custom(PipeSide* pipe, Cli* cmd_set, CliShellCustomMotd custom_motd);

#ifdef __cplusplus
}
#endif
