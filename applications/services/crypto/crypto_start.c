#include <cli/cli_i.h>

static void crypto_cli_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("crypto_cli", 1, cli, args, context);
}

void crypto_on_system_start() {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "crypto", CliCommandFlagDefault, crypto_cli_wrapper, NULL);
    furi_record_close(RECORD_CLI);
}
