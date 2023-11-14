#include <flipper_application/flipper_application.h>
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"
#include "../../cli_plugin_interface.h"
#include "../../../ui/scene_director.h"
#include "../../../services/config/config.h"

#define TOTP_CLI_RESET_CONFIRMATION_KEYWORD "YES"

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    UNUSED(args);
    TOTP_CLI_LOCK_UI(plugin_state);
    TOTP_CLI_PRINTF_WARNING(
        "As a result of reset all the settings and tokens will be permanently lost.\r\n");
    TOTP_CLI_PRINTF_WARNING("Do you really want to reset application?\r\n");
    TOTP_CLI_PRINTF_WARNING("Type \"" TOTP_CLI_RESET_CONFIRMATION_KEYWORD
                            "\" and hit <ENTER> to confirm:\r\n");
    FuriString* temp_str = furi_string_alloc();
    bool is_confirmed = totp_cli_read_line(cli, temp_str, false) &&
                        furi_string_cmpi_str(temp_str, TOTP_CLI_RESET_CONFIRMATION_KEYWORD) == 0;
    furi_string_free(temp_str);
    if(is_confirmed) {
        totp_config_file_reset(plugin_state);
        TOTP_CLI_PRINTF_SUCCESS("Application has been successfully reset to default.\r\n");
        TOTP_CLI_PRINTF_SUCCESS("Now application will be closed to apply all the changes.\r\n");
        totp_cli_force_close_app(plugin_state->event_queue);
    } else {
        TOTP_CLI_PRINTF_INFO("Action was not confirmed by user\r\n");
        TOTP_CLI_UNLOCK_UI(plugin_state);
    }
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Reset", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_reset_plugin_ep() {
    return &plugin_descriptor;
}