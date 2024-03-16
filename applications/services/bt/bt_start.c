#include <cli/cli_i.h>
#include "bt_service/bt.h"

static void bt_cli_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("bt_cli", 1, cli, args, context);
}

void bt_on_system_start() {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, RECORD_BT, CliCommandFlagDefault, bt_cli_wrapper, NULL);
    furi_record_close(RECORD_CLI);
}
