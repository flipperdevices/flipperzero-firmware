#include <cli/cli_i.h>

static void js_cli_execute_wrapper(Cli* cli, FuriString* args, void* context) {
    cli_plugin_wrapper("js_cli", 1, cli, args, context);
}

void js_app_on_system_start(void) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "js", CliCommandFlagDefault, js_cli_execute_wrapper, NULL);
    furi_record_close(RECORD_CLI);
}
