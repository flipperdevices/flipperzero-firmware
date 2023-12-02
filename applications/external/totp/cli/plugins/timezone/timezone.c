#include <lib/toolbox/args.h>
#include <flipper_application/flipper_application.h>
#include "../../../services/config/config.h"
#include "../../../ui/scene_director.h"
#include "../../cli_helpers.h"
#include "../../cli_plugin_interface.h"
#include "../../cli_shared_methods.h"

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    UNUSED(args);
    UNUSED(plugin_state);
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    FuriString* temp_str = furi_string_alloc();
    if(args_read_string_and_trim(args, temp_str)) {
        char* strtof_endptr;
        float tz = strtof(furi_string_get_cstr(temp_str), &strtof_endptr);
        if(*strtof_endptr == 0 && tz >= -12.75f && tz <= 12.75f) {
            TOTP_CLI_LOCK_UI(plugin_state);
            plugin_state->timezone_offset = tz;
            if(totp_config_file_update_timezone_offset(plugin_state)) {
                TOTP_CLI_PRINTF_SUCCESS("Timezone is set to %f\r\n", (double)tz);
            } else {
                TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE();
            }
            TOTP_CLI_UNLOCK_UI(plugin_state);
        } else {
            TOTP_CLI_PRINTF_ERROR("Invalid timezone offset\r\n");
        }
    } else {
        TOTP_CLI_PRINTF_INFO(
            "Current timezone offset is %f\r\n", (double)plugin_state->timezone_offset);
    }
    furi_string_free(temp_str);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Timezone", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_timezone_plugin_ep() {
    return &plugin_descriptor;
}