#include <flipper_application/flipper_application.h>
#include "../../cli_helpers.h"
#include "../../cli_plugin_interface.h"
#include "../../../version.h"

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    UNUSED(args);
    UNUSED(cli);
    UNUSED(plugin_state);
    TOTP_CLI_PRINTF(
        "%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\r\n",
        TOTP_APP_VERSION_MAJOR,
        TOTP_APP_VERSION_MINOR,
        TOTP_APP_VERSION_PATCH);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Version", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_version_plugin_ep() {
    return &plugin_descriptor;
}