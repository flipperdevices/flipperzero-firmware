#include "cli.h"
#include "cli_i.h"
#include "cli_commands.h"
#include "cli_ansi.h"
#include <toolbox/pipe.h>

#define TAG "cli"

struct Cli {
    CliCommandTree_t commands;
    FuriMutex* mutex;
};

Cli* cli_alloc(void) {
    Cli* cli = malloc(sizeof(Cli));
    CliCommandTree_init(cli->commands);
    cli->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    return cli;
}

void cli_add_command(
    Cli* cli,
    const char* name,
    CliCommandFlag flags,
    CliExecuteCallback callback,
    void* context) {
    cli_add_command_ex(cli, name, flags, callback, context, CLI_BUILTIN_COMMAND_STACK_SIZE);
}

void cli_add_command_ex(
    Cli* cli,
    const char* name,
    CliCommandFlag flags,
    CliExecuteCallback callback,
    void* context,
    size_t stack_size) {
    furi_check(cli);
    furi_check(name);
    furi_check(callback);

    FuriString* name_str;
    name_str = furi_string_alloc_set(name);
    // command cannot contain spaces
    furi_check(furi_string_search_char(name_str, ' ') == FURI_STRING_FAILURE);

    CliCommand command = {
        .context = context,
        .execute_callback = callback,
        .flags = flags,
        .stack_depth = stack_size,
    };

    furi_check(furi_mutex_acquire(cli->mutex, FuriWaitForever) == FuriStatusOk);
    CliCommandTree_set_at(cli->commands, name_str, command);
    furi_check(furi_mutex_release(cli->mutex) == FuriStatusOk);

    furi_string_free(name_str);
}

void cli_delete_command(Cli* cli, const char* name) {
    furi_check(cli);
    FuriString* name_str;
    name_str = furi_string_alloc_set(name);
    furi_string_trim(name_str);

    size_t name_replace;
    do {
        name_replace = furi_string_replace(name_str, " ", "_");
    } while(name_replace != FURI_STRING_FAILURE);

    furi_check(furi_mutex_acquire(cli->mutex, FuriWaitForever) == FuriStatusOk);
    CliCommandTree_erase(cli->commands, name_str);
    furi_check(furi_mutex_release(cli->mutex) == FuriStatusOk);

    furi_string_free(name_str);
}

bool cli_get_command(Cli* cli, FuriString* command, CliCommand* result) {
    furi_assert(cli);
    furi_check(furi_mutex_acquire(cli->mutex, FuriWaitForever) == FuriStatusOk);
    CliCommand* data = CliCommandTree_get(cli->commands, command);
    if(data) *result = *data;

    furi_check(furi_mutex_release(cli->mutex) == FuriStatusOk);

    return !!data;
}

void cli_lock_commands(Cli* cli) {
    furi_assert(cli);
    furi_check(furi_mutex_acquire(cli->mutex, FuriWaitForever) == FuriStatusOk);
}

void cli_unlock_commands(Cli* cli) {
    furi_assert(cli);
    furi_mutex_release(cli->mutex);
}

CliCommandTree_t* cli_get_commands(Cli* cli) {
    furi_assert(cli);
    return &cli->commands;
}

bool cli_is_pipe_broken_or_is_etx_next_char(PipeSide* side) {
    if(pipe_state(side) == PipeStateBroken) return true;
    if(!pipe_bytes_available(side)) return false;
    char c = getchar();
    return c == CliKeyETX;
}

void cli_print_usage(const char* cmd, const char* usage, const char* arg) {
    furi_check(cmd);
    furi_check(arg);
    furi_check(usage);

    printf("%s: illegal option -- %s\r\nusage: %s %s", cmd, arg, cmd, usage);
}

void cli_on_system_start(void) {
    Cli* cli = cli_alloc();
    cli_commands_init(cli);
    furi_record_create(RECORD_CLI, cli);
}
