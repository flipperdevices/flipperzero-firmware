#include "f0forth_i.h"
#include "f0forth_cli.h"

#include "lib/pforth/pforth.h"

F0ForthCliState* f0f_cli_state = NULL;

void f0forth_cmd(Cli* cli, FuriString* args, void* context) {
    furi_assert(context);
    F0ForthState* f0f_state = context;

    const char* source_name = NULL;

    if(furi_string_size(args) > 0) {
        source_name = furi_string_get_cstr(args);
    }

    if(furi_mutex_acquire(f0f_state->cli_running_mutex, FuriWaitForever) != FuriStatusOk) {
        cli_print_usage(F0FORTH_CMD, "Failed to acquire mutex!", furi_string_get_cstr(args));
        return;
    }

    f0f_cli_state = malloc(sizeof(F0ForthCliState));
    f0f_cli_state->cli = cli;

    pfDoForth(NULL, source_name, 0);

    free(f0f_cli_state);
    f0f_cli_state = NULL;

    furi_check(furi_mutex_release(f0f_state->cli_running_mutex) == FuriStatusOk);
}
