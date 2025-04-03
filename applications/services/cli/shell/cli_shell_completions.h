#pragma once

#include <furi.h>
#include <m-array.h>
#include "cli_shell_i.h"
#include "cli_shell_line.h"
#include "../cli.h"
#include "../cli_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CliShellCompletions CliShellCompletions;

CliShellCompletions* cli_shell_completions_alloc(Cli* cli, CliShell* shell, CliShellLine* line);

void cli_shell_completions_free(CliShellCompletions* completions);

extern CliShellKeyComboSet cli_shell_completions_key_combo_set;

#ifdef __cplusplus
}
#endif
