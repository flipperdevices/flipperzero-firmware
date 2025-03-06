#pragma once

#include "../cli.h"
#include <furi.h>
#include <toolbox/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_SHELL_STACK_SIZE (2 * 1024U)

/** Cli shell custom motd callback */
typedef void (*CliShellMotd)(void);

/**
 * @brief Start default cli shell
 * @param[in] pipe pipe from other side
 */
FuriThread* cli_shell_start(PipeSide* pipe);

/**
 * @brief Start custom cli shell with another command set and motd
 * @param[in] pipe pipe from other side
 * @param[in] name name for new shell thread
 * @param[in] cmd_set predefined command set which will be used by new shell
 * @param[in] custom_motd callback to function which can draw motd from otside
 */
FuriThread* cli_shell_start_custom(
    PipeSide* pipe,
    const char* name,
    Cli* cmd_set,
    CliShellMotd custom_motd);

/**
 * @brief Stop custom cli shell.
 * @param[in] thread Shell thread previously provided from cli_shell_start_custom
 */
void cli_shell_stop_custom(FuriThread* thread);

#ifdef __cplusplus
}
#endif
