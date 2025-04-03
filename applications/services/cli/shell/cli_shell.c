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
#include <storage/storage.h>

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

typedef enum {
    CliShellStorageEventMount,
    CliShellStorageEventUnmount,
} CliShellStorageEvent;

struct CliShell {
    Cli* cli;
    FuriEventLoop* event_loop;
    PipeSide* pipe;

    CliAnsiParser* ansi_parser;
    FuriEventLoopTimer* ansi_parsing_timer;

    Storage* storage;
    FuriPubSubSubscription* storage_subscription;
    FuriMessageQueue* storage_event_queue;

    void* components[CliShellComponentMAX];
};

typedef struct {
    CliCommand* command;
    PipeSide* pipe;
    FuriString* args;
} CliCommandThreadData;

static void cli_shell_data_available(PipeSide* pipe, void* context);
static void cli_shell_pipe_broken(PipeSide* pipe, void* context);

static void cli_shell_install_pipe(CliShell* cli_shell) {
    pipe_install_as_stdio(cli_shell->pipe);
    pipe_attach_to_event_loop(cli_shell->pipe, cli_shell->event_loop);
    pipe_set_callback_context(cli_shell->pipe, cli_shell);
    pipe_set_data_arrived_callback(cli_shell->pipe, cli_shell_data_available, 0);
    pipe_set_broken_callback(cli_shell->pipe, cli_shell_pipe_broken, 0);
}

static void cli_shell_detach_pipe(CliShell* cli_shell) {
    pipe_detach_from_event_loop(cli_shell->pipe);
    furi_thread_set_stdin_callback(NULL, NULL);
    furi_thread_set_stdout_callback(NULL, NULL);
}

// =========
// Execution
// =========

static int32_t cli_command_thread(void* context) {
    CliCommandThreadData* thread_data = context;
    if(!(thread_data->command->flags & CliCommandFlagDontAttachStdio))
        pipe_install_as_stdio(thread_data->pipe);

    thread_data->command->execute_callback(
        thread_data->pipe, thread_data->args, thread_data->command->context);

    fflush(stdout);
    return 0;
}

void cli_shell_execute_command(CliShell* cli_shell, FuriString* command) {
    // split command into command and args
    size_t space = furi_string_search_char(command, ' ');
    if(space == FURI_STRING_FAILURE) space = furi_string_size(command);
    FuriString* command_name = furi_string_alloc_set(command);
    furi_string_left(command_name, space);
    FuriString* args = furi_string_alloc_set(command);
    furi_string_right(args, space + 1);

    PluginManager* plugin_manager = NULL;
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

        // load external command
        if(command_data.flags & CliCommandFlagExternal) {
            plugin_manager =
                plugin_manager_alloc(PLUGIN_APP_ID, PLUGIN_API_VERSION, firmware_api_interface);
            FuriString* path = furi_string_alloc_printf(
                "%s/cli_%s.fal", CLI_COMMANDS_PATH, furi_string_get_cstr(command_name));
            uint32_t plugin_cnt_last = plugin_manager_get_count(plugin_manager);
            PluginManagerError error =
                plugin_manager_load_single(plugin_manager, furi_string_get_cstr(path));
            furi_string_free(path);

            if(error != PluginManagerErrorNone) {
                printf(ANSI_FG_RED "failed to load external command" ANSI_RESET);
                break;
            }

            const CliCommandDescriptor* plugin =
                plugin_manager_get_ep(plugin_manager, plugin_cnt_last);
            furi_assert(plugin);
            furi_check(furi_string_cmp_str(command_name, plugin->name) == 0);
            command_data.execute_callback = plugin->execute_callback;
            command_data.flags = plugin->flags | CliCommandFlagExternal;
            command_data.stack_depth = plugin->stack_depth;

            // external commands have to run in an external thread
            furi_check(!(command_data.flags & CliCommandFlagUseShellThread));
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

        if(command_data.flags & CliCommandFlagUseShellThread) {
            // run command in this thread
            command_data.execute_callback(cli_shell->pipe, args, command_data.context);
        } else {
            // run command in separate thread
            cli_shell_detach_pipe(cli_shell);
            CliCommandThreadData thread_data = {
                .command = &command_data,
                .pipe = cli_shell->pipe,
                .args = args,
            };
            FuriThread* thread = furi_thread_alloc_ex(
                furi_string_get_cstr(command_name),
                command_data.stack_depth,
                cli_command_thread,
                &thread_data);
            furi_thread_start(thread);
            furi_thread_join(thread);
            furi_thread_free(thread);
            cli_shell_install_pipe(cli_shell);
        }
    } while(0);

    furi_string_free(command_name);
    furi_string_free(args);

    // unlock loader
    if(loader) loader_unlock(loader);
    furi_record_close(RECORD_LOADER);

    // unload external command
    if(plugin_manager) plugin_manager_free(plugin_manager);
}

// ==============
// Event handlers
// ==============

static void cli_shell_storage_event(const void* message, void* context) {
    CliShell* cli_shell = context;
    const StorageEvent* event = message;

    if(event->type == StorageEventTypeCardMount) {
        CliShellStorageEvent cli_event = CliShellStorageEventMount;
        furi_check(
            furi_message_queue_put(cli_shell->storage_event_queue, &cli_event, 0) == FuriStatusOk);
    } else if(event->type == StorageEventTypeCardUnmount) {
        CliShellStorageEvent cli_event = CliShellStorageEventUnmount;
        furi_check(
            furi_message_queue_put(cli_shell->storage_event_queue, &cli_event, 0) == FuriStatusOk);
    }
}

static void cli_shell_storage_internal_event(FuriEventLoopObject* object, void* context) {
    CliShell* cli_shell = context;
    FuriMessageQueue* queue = object;
    CliShellStorageEvent event;
    furi_check(furi_message_queue_get(queue, &event, 0) == FuriStatusOk);

    if(event == CliShellStorageEventMount) {
        cli_enumerate_external_commands(cli_shell->cli);
    } else if(event == CliShellStorageEventUnmount) {
        cli_remove_external_commands(cli_shell->cli);
    } else {
        furi_crash();
    }
}

static void
    cli_shell_process_parser_result(CliShell* cli_shell, CliAnsiParserResult parse_result) {
    if(!parse_result.is_done) return;
    CliKeyCombo key_combo = parse_result.result;
    if(key_combo.key == CliKeyUnrecognized) return;

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
    cli_shell_process_parser_result(cli_shell, cli_ansi_parser_feed(cli_shell->ansi_parser, c));
}

static void cli_shell_timer_expired(void* context) {
    CliShell* cli_shell = context;
    cli_shell_process_parser_result(
        cli_shell, cli_ansi_parser_feed_timeout(cli_shell->ansi_parser));
}

// =======
// Helpers
// =======

static CliShell* cli_shell_alloc(PipeSide* pipe) {
    CliShell* cli_shell = malloc(sizeof(CliShell));

    cli_shell->cli = furi_record_open(RECORD_CLI);
    cli_shell->ansi_parser = cli_ansi_parser_alloc();
    cli_shell->pipe = pipe;

    cli_shell->components[CliShellComponentLine] = cli_shell_line_alloc(cli_shell);
    cli_shell->components[CliShellComponentCompletions] = cli_shell_completions_alloc(
        cli_shell->cli, cli_shell, cli_shell->components[CliShellComponentLine]);

    cli_shell->event_loop = furi_event_loop_alloc();
    cli_shell->ansi_parsing_timer = furi_event_loop_timer_alloc(
        cli_shell->event_loop, cli_shell_timer_expired, FuriEventLoopTimerTypeOnce, cli_shell);

    cli_shell_install_pipe(cli_shell);

    cli_shell->storage_event_queue = furi_message_queue_alloc(1, sizeof(CliShellStorageEvent));
    furi_event_loop_subscribe_message_queue(
        cli_shell->event_loop,
        cli_shell->storage_event_queue,
        FuriEventLoopEventIn,
        cli_shell_storage_internal_event,
        cli_shell);
    cli_shell->storage = furi_record_open(RECORD_STORAGE);
    cli_shell->storage_subscription = furi_pubsub_subscribe(
        storage_get_pubsub(cli_shell->storage), cli_shell_storage_event, cli_shell);

    return cli_shell;
}

static void cli_shell_free(CliShell* cli_shell) {
    furi_pubsub_unsubscribe(
        storage_get_pubsub(cli_shell->storage), cli_shell->storage_subscription);
    furi_record_close(RECORD_STORAGE);
    furi_event_loop_unsubscribe(cli_shell->event_loop, cli_shell->storage_event_queue);
    furi_message_queue_free(cli_shell->storage_event_queue);

    cli_shell_completions_free(cli_shell->components[CliShellComponentCompletions]);
    cli_shell_line_free(cli_shell->components[CliShellComponentLine]);

    cli_shell_detach_pipe(cli_shell);
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
