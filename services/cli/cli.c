// Original idea: https://github.com/br0ziliy

#include "cli.h"
#include <lib/toolbox/args.h>
#include "cli_common_helpers.h"
#include "commands/list/list.h"
#include "commands/add/add.h"
#include "commands/delete/delete.h"
#include "commands/timezone/timezone.h"

#define TOTP_CLI_COMMAND_NAME "totp"
#define TOTP_CLI_COMMAND_HELP "help"

static void totp_cli_print_unknown_command(FuriString* unknown_command) {
    TOTP_CLI_PRINTF(
        "Command \"%s\" is unknown. Use \"help\" command to get list of available commands.",
        furi_string_get_cstr(unknown_command));
}

static void totp_cli_print_help() {
    TOTP_CLI_PRINTF("Usage:\r\n");
    TOTP_CLI_PRINTF(TOTP_CLI_COMMAND_NAME " <command> <arguments>\r\n");
    TOTP_CLI_PRINTF("Command list:\r\n");
    TOTP_CLI_PRINTF("\t" TOTP_CLI_COMMAND_HELP " - print command usage help\r\n\r\n");
    totp_cli_command_list_print_help();
    totp_cli_command_delete_print_help();
    totp_cli_command_add_print_help();
    totp_cli_command_timezone_print_help();
}

static void totp_cli_handler(Cli* cli, FuriString* args, void* context) {
    PluginState* plugin_state = (PluginState*)context;

    FuriString* cmd = furi_string_alloc();

    args_read_string_and_trim(args, cmd);

    if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_HELP) == 0 || furi_string_empty(cmd)) {
        totp_cli_print_help();
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_ADD) == 0) {
        totp_cli_command_add_handle(plugin_state, args, cli);
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_LIST) == 0) {
        totp_cli_command_list_handle(plugin_state, cli);
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_DELETE) == 0) {
        totp_cli_command_delete_handle(plugin_state, args, cli);
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_TIMEZONE) == 0) {
        totp_cli_command_timezone_handle(plugin_state, args, cli);
    } else {
        totp_cli_print_unknown_command(cmd);
    }

    furi_string_free(cmd);
}

void totp_cli_register_command_handler(PluginState* plugin_state) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(
        cli, TOTP_CLI_COMMAND_NAME, CliCommandFlagParallelSafe, totp_cli_handler, plugin_state);
    furi_record_close(RECORD_CLI);
}

void totp_cli_unregister_command_handler() {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_delete_command(cli, TOTP_CLI_COMMAND_NAME);
    furi_record_close(RECORD_CLI);
}