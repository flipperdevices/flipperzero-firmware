#include "nfc_cli.h"
//#include "cli/nfc_cli_scanner.h"
#include "nfc_cli_commands.h"
#include "nfc_cli_command_processor.h"
// #include <furi.h>
// #include <furi_hal.h>
// #include <cli/shell/cli_shell.h>
// #include <cli/cli.h>
// #include <cli/cli_vcp.h>
// #include <cli/cli_commands.h>
// #include <lib/toolbox/args.h>
// #include <lib/toolbox/hex.h>
// #include <toolbox/pipe.h>
// #include <nfc/nfc.h>

// #include <furi_hal_nfc.h>

#define TAG "NfcCli"

// typedef struct {
//     Nfc* nfc;
//     Cli* nfc_cli;
//     FuriThread* shell;
// } NfcCliContext;

static void nfc_cli_print_usage(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);

    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t - turn field on\r\n");
    }
}

static void nfc_cli_field(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(args);
    UNUSED(context);

    furi_hal_nfc_low_power_mode_stop();
    furi_hal_nfc_poller_field_on();

    printf("Field is on. Don't leave device in this mode for too long.\r\n");
    printf("Press Ctrl+C to abort\r\n");

    while(!cli_app_should_stop(pipe)) {
        furi_delay_ms(50);
    }

    furi_hal_nfc_low_power_mode_start();
}

static void cli_shell_motd(void) {
    printf(ANSI_FG_BR_BLUE "\r\n"
                           "                                     0000      \r\n"
                           "                                     0000      \r\n"
                           "                             000      0000     \r\n"
                           "                             0000     00000    \r\n"
                           "                    000      00000     0000    \r\n"
                           "     0              0000      0000     00000   \r\n"
                           "   000000           0000      00000     0000   \r\n"
                           "   00000000          0000      0000     0000   \r\n"
                           "   0000000000        0000      00000    0000   \r\n"
                           "   0000 00000000     00000     00000    0000   \r\n"
                           "   0000    0000000   00000     00000    0000   \r\n"
                           "   0000      000000000000      0000     0000   \r\n"
                           "   00000        000000000     00000     0000   \r\n"
                           "     00           000000      0000     00000   \r\n"
                           "                     00      00000     0000    \r\n"
                           "                             0000     00000    \r\n"
                           "                             000      0000     \r\n"
                           "                                     0000      \r\n"
                           "                                     0005      \r\n"
                           "\r\n" ANSI_FG_BR_WHITE "Welcome to NFC Command Line Interface!\r\n"
                           "Run `help` or `?` to list available commands\r\n" ANSI_RESET);
}

// static void test2(PipeSide* pipe, FuriString* args, void* context) {
//     UNUSED(pipe);
//     UNUSED(args);
//     UNUSED(context);
//     printf("test2\r\n");
// }

/* void print_cmd_descriptor(const NfcCliCommandDescriptor* cmd) {
    printf("Name: %s\r\n", cmd->name);
    if(cmd->description) printf("Description: %s\r\n", cmd->description);

    printf("Actions\r\nCount:%d\r\n", cmd->action_count);
    for(size_t i = 0; i < cmd->action_count; i++) {
        const NfcCliActionDescriptor* action = cmd->actions[i];

        printf("Action: %s\r\n", action->name);
        printf("Keys\r\nCount:%d\r\n", action->key_count);
        for(size_t i = 0; i < action->key_count; i++) {
            const NfcCliKeyDescriptor* key = &action->keys[i];
            if(key->long_name) printf("Long: %s\r\n", key->long_name ? key->long_name : "--");
            if(key->short_name) printf("Short: %s\r\n", key->short_name ? key->short_name : "--");
            printf("\r\n");
        }
        printf("\r\n");
    }
} */

/* static void test3(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    printf("Args: %s\r\n", furi_string_get_cstr(args));
    for(uint8_t i = 0; i < nfc_cli_command_get_count(); i++) {
        const NfcCliCommandDescriptor* cmd = nfc_cli_command_get_by_index(i);
        print_cmd_descriptor(cmd);
    }
} */

static void nfc_cli_command_help(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    cli_enumerate(context);
}

static void nfc_cli_exit(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    NfcCliContext* instance = context;
    cli_shell_stop_custom(instance->shell);
    printf("Exiting NFC shell...\r\n");
}

static void nfc_cli_subscribe_commands(NfcCliContext* instance) {
    size_t cnt = nfc_cli_command_get_count();
    for(size_t i = 0; i < cnt; i++) {
        const NfcCliCommandDescriptor* cmd = nfc_cli_command_get_by_index(i);
        CliExecuteCallback callback = nfc_cli_command_get_execute(cmd);
        if(callback != NULL) {
            const char* name = nfc_cli_command_get_name(cmd);
            cli_add_command(
                instance->nfc_cli,
                name,
                CliCommandFlagParallelUnsafe,
                callback,
                instance->processor_context);
        }
    }
}

NfcCliContext* nfc_cli_alloc() {
    NfcCliContext* instance = malloc(sizeof(NfcCliContext));
    instance->nfc = nfc_alloc();
    instance->processor_context = nfc_cli_command_processor_alloc(instance->nfc);

    instance->nfc_cli = cli_alloc();

    cli_add_command(
        instance->nfc_cli, "?", CliCommandFlagDefault, nfc_cli_command_help, instance->nfc_cli);
    cli_add_command(
        instance->nfc_cli, "help", CliCommandFlagDefault, nfc_cli_command_help, instance->nfc_cli);
    cli_add_command(instance->nfc_cli, "exit", CliCommandFlagDefault, nfc_cli_exit, instance);
    cli_add_command(instance->nfc_cli, "field", CliCommandFlagParallelUnsafe, nfc_cli_field, NULL);
    /*   cli_add_command(
        instance->nfc_cli, "scanner", CliCommandFlagParallelUnsafe, nfc_cli_scanner, instance); */

    nfc_cli_subscribe_commands(instance);

    return instance;
}

void nfc_cli_free(NfcCliContext* instance) {
    furi_assert(instance);
    furi_thread_free(instance->shell);
    cli_free(instance->nfc_cli);
    nfc_free(instance->nfc);
    free(instance);
}

void execute(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    UNUSED(nfc_cli_print_usage);

    NfcCliContext* instance = nfc_cli_alloc();

    instance->shell = cli_shell_start_custom(pipe, "NfcShell", instance->nfc_cli, cli_shell_motd);
    furi_thread_join(instance->shell);

    nfc_cli_free(instance);
}

///TODO: try to reduce stack
CLI_COMMAND_INTERFACE(nfc, execute, CliCommandFlagDefault, 1024);
