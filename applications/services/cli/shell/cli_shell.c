#include "cli_shell.h"
#include "cli_shell_i.h"
#include "../cli_ansi.h"
#include "../cli_i.h"
#include "../cli_commands.h"
#include "cli_shell_line.h"
#include "cli_shell_completions.h"
#include <stdio.h>
#include <furi_hal_version.h>
#include <m-array.h>
#include <loader/loader.h>
#include <toolbox/pipe.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <loader/firmware_api/firmware_api.h>

#define TAG "CliShell"

#define ANSI_TIMEOUT_MS 10

typedef enum {
    CliShellComponentCompletions,
    CliShellComponentLine,
    CliShellComponentMAX, //<! do not use
} CliShellComponent;

CliShellKeyComboSet* component_key_combo_sets[] = {
    [CliShellComponentCompletions] = &cli_shell_completions_key_combo_set,
    [CliShellComponentLine] = &cli_shell_line_key_combo_set,
};
static_assert(CliShellComponentMAX == COUNT_OF(component_key_combo_sets));

struct CliShell {
    Cli* cli;
    FuriEventLoop* event_loop;
    PipeSide* pipe;

    CliAnsiParser* ansi_parser;
    FuriEventLoopTimer* ansi_parsing_timer;

    void* components[CliShellComponentMAX];
};

typedef struct {
    CliCommand* command;
    PipeSide* pipe;
    FuriString* args;
} CliCommandThreadData;

// =========
// Execution
// =========

void cli_shell_execute_command(CliShell* cli_shell, FuriString* command) {
    // split command into command and args
    size_t space = furi_string_search_char(command, ' ');
    if(space == FURI_STRING_FAILURE) space = furi_string_size(command);
    FuriString* command_name = furi_string_alloc_set(command);
    furi_string_left(command_name, space);
    FuriString* args = furi_string_alloc_set(command);
    furi_string_right(args, space + 1);

    Loader* loader = NULL;
    CliCommand command_data;

    do {
        // find handler
        if(!cli_get_command(cli_shell->cli, command_name, &command_data)) {
            printf(
                ANSI_FG_RED "could not find command `%s`, try `help`" ANSI_RESET,
                furi_string_get_cstr(command_name));
            break;
        }

        // lock loader
        if(!(command_data.flags & CliCommandFlagParallelSafe)) {
            loader = furi_record_open(RECORD_LOADER);
            bool success = loader_lock(loader);
            if(!success) {
                printf(ANSI_FG_RED
                       "this command cannot be run while an application is open" ANSI_RESET);
                break;
            }
        }

        command_data.execute_callback(cli_shell->pipe, args, command_data.context);
    } while(0);

    furi_string_free(command_name);
    furi_string_free(args);

    // unlock loader
    if(loader) loader_unlock(loader);
    furi_record_close(RECORD_LOADER);
}

// ==============
// Event handlers
// ==============

static void cli_shell_process_key(CliShell* cli_shell, CliKeyCombo key_combo) {
    for(size_t i = 0; i < CliShellComponentMAX; i++) { // -V1008
        CliShellKeyComboSet* set = component_key_combo_sets[i];
        void* component_context = cli_shell->components[i];

        for(size_t j = 0; j < set->count; j++) {
            if(set->records[j].combo.modifiers == key_combo.modifiers &&
               set->records[j].combo.key == key_combo.key)
                if(set->records[j].action(key_combo, component_context)) return;
        }

        if(set->fallback)
            if(set->fallback(key_combo, component_context)) return;
    }
}

static void cli_shell_pipe_broken(PipeSide* pipe, void* context) {
    // allow commands to be processed before we stop the shell
    if(pipe_bytes_available(pipe)) return;

    CliShell* cli_shell = context;
    furi_event_loop_stop(cli_shell->event_loop);
}

static void cli_shell_data_available(PipeSide* pipe, void* context) {
    UNUSED(pipe);
    CliShell* cli_shell = context;

    furi_event_loop_timer_start(cli_shell->ansi_parsing_timer, furi_ms_to_ticks(ANSI_TIMEOUT_MS));

    // process ANSI escape sequences
    int c = getchar();
    furi_assert(c >= 0);
    CliAnsiParserResult parse_result = cli_ansi_parser_feed(cli_shell->ansi_parser, c);
    if(!parse_result.is_done) return;
    CliKeyCombo key_combo = parse_result.result;
    if(key_combo.key == CliKeyUnrecognized) return;

    cli_shell_process_key(cli_shell, key_combo);
}

static void cli_shell_timer_expired(void* context) {
    CliShell* cli_shell = context;
    CliAnsiParserResult parse_result = cli_ansi_parser_feed_timeout(cli_shell->ansi_parser);
    if(!parse_result.is_done) return;
    CliKeyCombo key_combo = parse_result.result;
    if(key_combo.key == CliKeyUnrecognized) return;

    cli_shell_process_key(cli_shell, key_combo);
}

// =======
// Helpers
// =======

static CliShell* cli_shell_alloc(PipeSide* pipe) {
    CliShell* cli_shell = malloc(sizeof(CliShell));

    cli_shell->cli = furi_record_open(RECORD_CLI);
    cli_shell->ansi_parser = cli_ansi_parser_alloc();
    cli_shell->pipe = pipe;
    pipe_install_as_stdio(cli_shell->pipe);

    cli_shell->components[CliShellComponentLine] = cli_shell_line_alloc(cli_shell);
    cli_shell->components[CliShellComponentCompletions] = cli_shell_completions_alloc(
        cli_shell->cli, cli_shell, cli_shell->components[CliShellComponentLine]);

    cli_shell->event_loop = furi_event_loop_alloc();
    cli_shell->ansi_parsing_timer = furi_event_loop_timer_alloc(
        cli_shell->event_loop, cli_shell_timer_expired, FuriEventLoopTimerTypeOnce, cli_shell);
    pipe_attach_to_event_loop(cli_shell->pipe, cli_shell->event_loop);

    pipe_set_callback_context(cli_shell->pipe, cli_shell);
    pipe_set_data_arrived_callback(cli_shell->pipe, cli_shell_data_available, 0);
    pipe_set_broken_callback(cli_shell->pipe, cli_shell_pipe_broken, 0);

    return cli_shell;
}

static void cli_shell_free(CliShell* cli_shell) {
    cli_shell_completions_free(cli_shell->components[CliShellComponentCompletions]);
    cli_shell_line_free(cli_shell->components[CliShellComponentLine]);

    pipe_detach_from_event_loop(cli_shell->pipe);
    furi_event_loop_timer_free(cli_shell->ansi_parsing_timer);
    furi_event_loop_free(cli_shell->event_loop);
    pipe_free(cli_shell->pipe);
    cli_ansi_parser_free(cli_shell->ansi_parser);
    furi_record_close(RECORD_CLI);
    free(cli_shell);
}

static void cli_shell_motd(void) {
    printf(ANSI_FLIPPER_BRAND_ORANGE
           "\r\n"
           "              _.-------.._                    -,\r\n"
           "          .-\"```\"--..,,_/ /`-,               -,  \\ \r\n"
           "       .:\"          /:/  /'\\  \\     ,_...,  `. |  |\r\n"
           "      /       ,----/:/  /`\\ _\\~`_-\"`     _;\r\n"
           "     '      / /`\"\"\"'\\ \\ \\.~`_-'      ,-\"'/ \r\n"
           "    |      | |  0    | | .-'      ,/`  /\r\n"
           "   |    ,..\\ \\     ,.-\"`       ,/`    /\r\n"
           "  ;    :    `/`\"\"\\`           ,/--==,/-----,\r\n"
           "  |    `-...|        -.___-Z:_______J...---;\r\n"
           "  :         `                           _-'\r\n"
           " _L_  _     ___  ___  ___  ___  ____--\"`___  _     ___\r\n"
           "| __|| |   |_ _|| _ \\| _ \\| __|| _ \\   / __|| |   |_ _|\r\n"
           "| _| | |__  | | |  _/|  _/| _| |   /  | (__ | |__  | |\r\n"
           "|_|  |____||___||_|  |_|  |___||_|_\\   \\___||____||___|\r\n"
           "\r\n" ANSI_FG_BR_WHITE "Welcome to Flipper Zero Command Line Interface!\r\n"
           "Read the manual: https://docs.flipper.net/development/cli\r\n"
           "Run `help` or `?` to list available commands\r\n"
           "\r\n" ANSI_RESET);

    const Version* firmware_version = furi_hal_version_get_firmware_version();
    if(firmware_version) {
        printf(
            "Firmware version: %s %s (%s%s built on %s)\r\n",
            version_get_gitbranch(firmware_version),
            version_get_version(firmware_version),
            version_get_githash(firmware_version),
            version_get_dirty_flag(firmware_version) ? "-dirty" : "",
            version_get_builddate(firmware_version));
    }
}

static int32_t cli_shell_thread(void* context) {
    PipeSide* pipe = context;

    // Sometimes, the other side closes the pipe even before our thread is started. Although the
    // rest of the code will eventually find this out if this check is removed, there's no point in
    // wasting time.
    if(pipe_state(pipe) == PipeStateBroken) return 0;

    CliShell* cli_shell = cli_shell_alloc(pipe);

    FURI_LOG_D(TAG, "Started");
    cli_shell_motd();
    cli_shell_line_prompt(cli_shell->components[CliShellComponentLine]);

    furi_event_loop_run(cli_shell->event_loop);

    FURI_LOG_D(TAG, "Stopped");

    cli_shell_free(cli_shell);
    return 0;
}

// ==========
// Public API
// ==========

FuriThread* cli_shell_start(PipeSide* pipe) {
    FuriThread* thread =
        furi_thread_alloc_ex("CliShell", CLI_SHELL_STACK_SIZE, cli_shell_thread, pipe);
    furi_thread_start(thread);
    return thread;
}
