/**
 * @file cli.h
 * API for registering commands with the CLI
 */

#pragma once
#include <furi.h>
#include <m-array.h>
#include "cli_ansi.h"
#include <toolbox/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_CLI "cli"

typedef enum {
    CliCommandFlagDefault = 0, /**< Default */
    CliCommandFlagParallelSafe = (1 << 0), /**< Safe to run in parallel with other apps */
    CliCommandFlagInsomniaSafe = (1 << 1), /**< Safe to run with insomnia mode on */
    CliCommandFlagDontAttachStdio = (1 << 2), /**< Do no attach I/O pipe to thread stdio */
    CliCommandFlagUseShellThread =
        (1
         << 3), /**< Don't start a separate thread to run the command in. Incompatible with DontAttachStdio */

    // internal flags (do not set them yourselves!)

    CliCommandFlagExternal = (1 << 4), /**< The command comes from a .fal file */
} CliCommandFlag;

/** Cli type anonymous structure */
typedef struct Cli Cli;

/** 
 * @brief CLI execution callback pointer. Implement this interface and use
 *        `add_cli_command`.
 * 
 * This callback will be called from a separate thread spawned just for your
 * command. The pipe will be installed as the thread's stdio, so you can use
 * `printf`, `getchar` and other standard functions to communicate with the
 * user.
 * 
 * @param [in] pipe     Pipe that can be used to send and receive data. If
 *                      `CliCommandFlagDontAttachStdio` was not set, you can
 *                      also use standard C functions (printf, getc, etc.) to
 *                      access this pipe.
 * @param [in] args     String with what was passed after the command
 * @param [in] context  Whatever you provided to `cli_add_command`
 */
typedef void (*CliExecuteCallback)(PipeSide* pipe, FuriString* args, void* context);

/**
 * @brief Registers a command with the CLI. Provides less options than the `_ex`
 * counterpart.
 *
 * @param [in] cli       Pointer to CLI instance
 * @param [in] name      Command name
 * @param [in] flags     CliCommandFlag
 * @param [in] callback  Callback function
 * @param [in] context   Custom context
 */
void cli_add_command(
    Cli* cli,
    const char* name,
    CliCommandFlag flags,
    CliExecuteCallback callback,
    void* context);

/**
 * @brief Registers a command with the CLI. Provides more options than the
 * non-`_ex` counterpart.
 *
 * @param [in] cli        Pointer to CLI instance
 * @param [in] name       Command name
 * @param [in] flags      CliCommandFlag
 * @param [in] callback   Callback function
 * @param [in] context    Custom context
 * @param [in] stack_size Thread stack size
 */
void cli_add_command_ex(
    Cli* cli,
    const char* name,
    CliCommandFlag flags,
    CliExecuteCallback callback,
    void* context,
    size_t stack_size);

/**
 * @brief Deletes a cli command
 *
 * @param [in] cli   pointer to cli instance
 * @param [in] name  command name
 */
void cli_delete_command(Cli* cli, const char* name);

/**
 * @brief Unregisters all external commands
 * 
 * @param [in] cli pointer to the cli instance
 */
void cli_remove_external_commands(Cli* cli);

/**
 * @brief Reloads the list of externally available commands
 * 
 * @param [in] cli pointer to cli instance
 */
void cli_enumerate_external_commands(Cli* cli);

/**
 * @brief Detects if Ctrl+C has been pressed or session has been terminated
 * 
 * @param [in] side Pointer to pipe side given to the command thread
 * @warning This function also assumes that the pipe is installed as the
 *          thread's stdio
 * @warning This function will consume 1 byte from the pipe
 */
bool cli_is_pipe_broken_or_is_etx_next_char(PipeSide* side);

/** Print unified cmd usage tip
 *
 * @param      cmd    cmd name
 * @param      usage  usage tip
 * @param      arg    arg passed by user
 */
void cli_print_usage(const char* cmd, const char* usage, const char* arg);

#ifdef __cplusplus
}
#endif
