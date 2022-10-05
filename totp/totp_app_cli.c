#include <furi.h>
#include <cli/cli.h>
#include <storage/storage.h>

void totp_cli_print_usage() {
    printf("Usage:\r\n");
    printf("totp <cmd> <args>\r\n");
    printf("Cmd list:\r\n");
    printf("\tadd <secret:string> <name:string>\t - Add new TOTP secret\r\n");
    printf("\tremove <name:string>\t - Remove TOTP token\r\n");
    printf("\reset\t - Reset app to default (reset PIN and removes all tokens)\r\n");
};

static void totp_cli(Cli* cli, string_t args, void* context) {
    UNUSED(cli);
    UNUSED(args);
    UNUSED(context);
    totp_cli_print_usage();
    // TODO: implement add\remove\reset
}

void totp_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = furi_record_open(RECORD_CLI);

    cli_add_command(cli, "totp", CliCommandFlagDefault, totp_cli, NULL);

    furi_record_close(RECORD_CLI);
#else
    UNUSED(totp_cli);
#endif
}
