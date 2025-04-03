#pragma once

#include <furi.h>
#include <toolbox/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_SHELL_STACK_SIZE (4 * 1024U)

FuriThread* cli_shell_start(PipeSide* pipe);

#ifdef __cplusplus
}
#endif
