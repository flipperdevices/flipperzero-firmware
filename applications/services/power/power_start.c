#include <cli/cli_i.h>

static void power_cli_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("power_cli", 1, cli, args, context);
}

void power_on_system_start(void) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "power", CliCommandFlagParallelSafe, power_cli_wrapper, NULL);
    furi_record_close(RECORD_CLI);
}
