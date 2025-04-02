/**
 * @file cli_i.h
 * Internal API for getting commands registered with the CLI
 */

#pragma once

#include <furi.h>
#include <m-bptree.h>
#include "cli.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_BUILTIN_COMMAND_STACK_SIZE (3 * 1024U)

typedef struct {
    void* context; //<! Context passed to callbacks
    CliExecuteCallback execute_callback; //<! Callback for command execution
    CliCommandFlag flags;
    size_t stack_depth;
} CliCommand;

#define CLI_COMMANDS_TREE_RANK 4

// -V:BPTREE_DEF2:1103
// -V:BPTREE_DEF2:524
BPTREE_DEF2(
    CliCommandTree,
    CLI_COMMANDS_TREE_RANK,
    FuriString*,
    FURI_STRING_OPLIST,
    CliCommand,
    M_POD_OPLIST);
#define M_OPL_CliCommandTree_t() BPTREE_OPLIST(CliCommandTree, M_POD_OPLIST)

bool cli_get_command(Cli* cli, FuriString* command, CliCommand* result);

void cli_lock_commands(Cli* cli);

void cli_unlock_commands(Cli* cli);

/**
 * @warning Surround calls to this function with `cli_[un]lock_commands`
 */
CliCommandTree_t* cli_get_commands(Cli* cli);

#ifdef __cplusplus
}
#endif
