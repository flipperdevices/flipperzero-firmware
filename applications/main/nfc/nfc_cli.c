#include <furi.h>
#include <furi_hal.h>
#include <cli/shell/cli_shell.h>
#include <cli/cli.h>
#include <cli/cli_vcp.h>
#include <cli/cli_commands.h>
#include <lib/toolbox/args.h>
#include <lib/toolbox/hex.h>
#include <toolbox/pipe.h>

#include <furi_hal_nfc.h>

#define FLAG_EVENT (1 << 10)

static void nfc_cli_print_usage(void) {
    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t - turn field on\r\n");
    }
}

static void nfc_cli_field(PipeSide* pipe, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_hal_ready() != FuriHalNfcErrorNone) {
        printf("NFC chip failed to start\r\n");
        return;
    }

    furi_hal_nfc_acquire();
    furi_hal_nfc_low_power_mode_stop();
    furi_hal_nfc_poller_field_on();

    printf("Field is on. Don't leave device in this mode for too long.\r\n");
    printf("Press Ctrl+C to abort\r\n");

    while(!cli_app_should_stop(pipe)) {
        furi_delay_ms(50);
    }

    furi_hal_nfc_low_power_mode_start();
    furi_hal_nfc_release();
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

static void test1(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    printf("test1\r\n");
}

static void test2(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    printf("test2\r\n");
}

static void test3(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    printf("test3\r\n");
}

extern void cli_command_help(PipeSide* pipe, FuriString* args, void* context);

static void nfc_cli_exit(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);

    CliVcp* cli_vcp = furi_record_open(RECORD_CLI_VCP);
    cli_vcp_restore_default_shell(cli_vcp);
    furi_record_close(RECORD_CLI_VCP);

    //Cli* nfc_cli = context;
    //cli_free(nfc_cli);
}

//nfc_cli_custom_command_set
void nfc_cli(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    UNUSED(nfc_cli_field);
    UNUSED(nfc_cli_print_usage);

    CliVcp* cli_vcp = furi_record_open(RECORD_CLI_VCP);

    Cli* custom_cli = cli_alloc();
    cli_add_command(custom_cli, "?", CliCommandFlagDefault, cli_command_help, custom_cli);
    cli_add_command(custom_cli, "help", CliCommandFlagDefault, cli_command_help, custom_cli);
    cli_add_command(custom_cli, "exit", CliCommandFlagDefault, nfc_cli_exit, custom_cli);
    cli_add_command(custom_cli, "test1", CliCommandFlagParallelUnsafe, test1, NULL);
    cli_add_command(custom_cli, "test2", CliCommandFlagParallelUnsafe, test2, NULL);
    cli_add_command(custom_cli, "test3", CliCommandFlagParallelUnsafe, test3, NULL);

    cli_vcp_define_custom_shell(cli_vcp, custom_cli, cli_shell_motd);

    furi_record_close(RECORD_CLI_VCP);
}

void nfc_on_system_start(void) {
#ifdef SRV_CLI
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "nfc", CliCommandFlagParallelUnsafe, nfc_cli, NULL);
    furi_record_close(RECORD_CLI);
#else
    UNUSED(nfc_cli);
#endif
}
