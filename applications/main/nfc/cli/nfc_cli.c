#include "nfc_cli.h"
#include "nfc_cli_commands.h"
#include "nfc_cli_command_processor.h"

#define TAG "NfcCli"

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
    //cli_add_command(instance->nfc_cli, "field", CliCommandFlagParallelUnsafe, nfc_cli_field, NULL);
    nfc_cli_subscribe_commands(instance);

    return instance;
}

void nfc_cli_free(NfcCliContext* instance) {
    furi_assert(instance);
    furi_thread_free(instance->shell);
    nfc_cli_command_processor_free(instance->processor_context);
    cli_free(instance->nfc_cli);
    nfc_free(instance->nfc);
    free(instance);
}

void nfc_cli_execute(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);

    NfcCliContext* instance = nfc_cli_alloc();

    instance->shell = cli_shell_start_custom(pipe, "NfcShell", instance->nfc_cli, cli_shell_motd);
    furi_thread_join(instance->shell);

    nfc_cli_free(instance);
}

///TODO: try to reduce stack
CLI_COMMAND_INTERFACE(nfc, nfc_cli_execute, CliCommandFlagDefault, 1024);
