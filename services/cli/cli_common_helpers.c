#include "cli_common_helpers.h"
#include <cli/cli.h>

void totp_cli_print_invalid_arguments() {
    TOTP_CLI_PRINTF("Invalid command arguments. use \"help\" command to get list of available commands");
}

bool totp_cli_ensure_authenticated(PluginState* plugin_state, Cli* cli) {
    if (plugin_state->current_scene == TotpSceneAuthentication) {
        TOTP_CLI_PRINTF("Pleases enter PIN on your flipper device\r\n");
        
        while (plugin_state->current_scene == TotpSceneAuthentication && !cli_cmd_interrupt_received(cli)) {
            furi_delay_tick(0);
        }

        TOTP_CLI_PRINTF("\033[A\33[2K\r");
        fflush(stdout);

        if (plugin_state->current_scene == TotpSceneAuthentication) {
            return false;
        }
    }

    return true;
}