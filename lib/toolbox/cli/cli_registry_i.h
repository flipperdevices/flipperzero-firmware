/**
 * @file cli_registry_i.h
 * Internal API for getting commands registered with the CLI
 */

#pragma once

#include <furi.h>
#include <m-bptree.h>
#include "cli_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_BUILTIN_COMMAND_STACK_SIZE (4 * 1024U)

typedef struct {
    void* context; //<! Context passed to callbacks
    CliCommandExecuteCallback execute_callback; //<! Callback for command execution
    CliCommandFlag flags;
    size_t stack_depth;
} CliRegistryCommand;

#define CLI_COMMANDS_TREE_RANK 4

// -V:BPTREE_DEF2:1103
// -V:BPTREE_DEF2:524
BPTREE_DEF2(
    CliCommandTree,
    CLI_COMMANDS_TREE_RANK,
    FuriString*,
    FURI_STRING_OPLIST,
    CliRegistryCommand,
    M_POD_OPLIST);

#define M_OPL_CliCommandTree_t() BPTREE_OPLIST2(CliCommandTree, FURI_STRING_OPLIST, M_POD_OPLIST)

bool cli_registry_get_command(
    CliRegistry* registry,
    FuriString* command,
    CliRegistryCommand* result);

void cli_registry_lock(CliRegistry* registry);

void cli_registry_unlock(CliRegistry* registry);

/**
 * @warning Surround calls to this function with `cli_registry_[un]lock`
 */
CliCommandTree_t* cli_registry_get_commands(CliRegistry* registry);

#ifdef __cplusplus
}
#endif
