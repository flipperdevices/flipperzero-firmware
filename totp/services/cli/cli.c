// Original idea: https://github.com/br0ziliy

#include "cli.h"
#include <lib/toolbox/args.h>
#include "commands/list/list.h"
#include "commands/add/add.h"
#include "commands/delete/delete.h"

#define TOTP_CLI_COMMAND_NAME "totp"

static void totp_cli_print_unknown_command(FuriString* unknown_command) {
    printf("Command \"%s\" is unknown. Use \"help\" command to get list of available commands.", furi_string_get_cstr(unknown_command));
}

static void totp_cli_print_help() {
    printf("Usage:\r\n");
    printf("totp <command> <arguments>\r\n");
    printf("Command list:\r\n");
    printf("\thelp - print command usage help\r\n");
    printf("\tlist - list all tokens\r\n");
    printf("\tdelete <INDEX> [-f] - delete token\r\n");
    printf("\t\t<INDEX> - token index in the list\r\n");
    printf("\t\t-f - [OPTIONAL] force command to do not ask user for interactive confirmation\r\n");
    printf("\tadd <NAME> <SECRET> [-a <ALGO>] [-d <DIGITS>] - add new token\r\n");
    printf("\t\t<NAME>   - token name\r\n");
    printf("\t\t<SECRET> - Base32 token secret\r\n");
    printf("\t\t<ALGO>   - [OPTIONAL] token hashing algorithm, could be one of: sha1, sha256, sha512; default: sha1\r\n");
    printf("\t\t<DIGITS> - [OPTIONAL] number of digits to generate, one of: 6, 8; default: 6\r\n\r\n");
}

static void totp_cli_print_unauthenticated() {
    printf("Pleases enter PIN on your flipper device\r\n");
}

static void totp_cli_handler(Cli* cli, FuriString* args, void* context) {
    PluginState* plugin_state = (PluginState* )context;

    if (plugin_state->current_scene == TotpSceneAuthentication) {
        totp_cli_print_unauthenticated();
        
        while (plugin_state->current_scene == TotpSceneAuthentication && !cli_cmd_interrupt_received(cli)) {
            furi_delay_tick(0);
        }

        if (plugin_state->current_scene == TotpSceneAuthentication) {
            return;
        }
    }

    FuriString* cmd = furi_string_alloc();

    args_read_string_and_trim(args, cmd);

    if(furi_string_cmp_str(cmd, "help") == 0 || furi_string_empty(cmd)) {
        totp_cli_print_help();
    } else if(furi_string_cmp_str(cmd, "add") == 0) {
        totp_cli_handle_add_command(plugin_state, args);
    } else if(furi_string_cmp_str(cmd, "list") == 0) {
        totp_cli_handle_list_command(plugin_state);
    } else if(furi_string_cmp_str(cmd, "delete") == 0) {
        totp_cli_handle_delete_command(plugin_state, args, cli);
    } else {
        totp_cli_print_unknown_command(cmd);
    }

    furi_string_free(cmd);
}

void totp_cli_register_command_handler(PluginState* plugin_state) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, TOTP_CLI_COMMAND_NAME, CliCommandFlagParallelSafe, totp_cli_handler, plugin_state);
    furi_record_close(RECORD_CLI);
}

void totp_cli_unregister_command_handler() {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_delete_command(cli, TOTP_CLI_COMMAND_NAME);
    furi_record_close(RECORD_CLI);
}