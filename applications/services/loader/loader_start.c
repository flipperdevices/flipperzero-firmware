#include <cli/cli_i.h>
#include "loader.h"

static void loader_cli_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("loader_cli", 1, cli, args, context);
}

void loader_on_system_start() {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, RECORD_LOADER, CliCommandFlagParallelSafe, loader_cli_wrapper, NULL);
    furi_record_close(RECORD_CLI);
}
