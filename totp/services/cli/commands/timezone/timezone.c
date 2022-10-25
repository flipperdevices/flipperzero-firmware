#include "timezone.h"
#include <lib/toolbox/args.h>
#include "../../../config/config.h"
#include "../../../../scenes/scene_director.h"
#include "../../cli_common_helpers.h"

#define TOTP_CLI_COMMAND_TIMEZONE_ARG_TIMEZONE "TIMEZONE"

void totp_cli_command_timezone_print_help() {
    TOTP_CLI_PRINTF("\t" TOTP_CLI_COMMAND_TIMEZONE " " TOTP_CLI_OPTIONAL_PARAM(TOTP_CLI_ARG(TOTP_CLI_COMMAND_TIMEZONE_ARG_TIMEZONE)) "\r\n");
    TOTP_CLI_PRINTF("\t\t" TOTP_CLI_ARG(TOTP_CLI_COMMAND_TIMEZONE_ARG_TIMEZONE) " - " TOTP_CLI_OPTIONAL_PARAM_MARK " timezone offset in hours to be set, if not provided then current timezone offset will be printed\r\n\r\n");
}

void totp_cli_command_timezone_handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if (!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    FuriString* temp_str = furi_string_alloc();
    if (args_read_string_and_trim(args, temp_str)) {
        float tz = strtof(furi_string_get_cstr(temp_str), NULL);
        if (tz >= -12.75f && tz <= 12.75f) {
            plugin_state->timezone_offset = tz;
            totp_config_file_update_timezone_offset(tz);
            TOTP_CLI_PRINTF("Timezone is set to %f\r\n", tz);
            if (plugin_state->current_scene == TotpSceneGenerateToken) {
                totp_scene_director_activate_scene(plugin_state, TotpSceneNone, NULL);
                totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, NULL);
            } else if (plugin_state->current_scene == TotpSceneAppSettings) {
                totp_scene_director_activate_scene(plugin_state, TotpSceneNone, NULL);
                totp_scene_director_activate_scene(plugin_state, TotpSceneAppSettings, NULL);
            }
        } else {
            TOTP_CLI_PRINTF("Invalid timezone offset\r\n");
        }
    } else {
        TOTP_CLI_PRINTF("Current timezone offset is %f\r\n", plugin_state->timezone_offset);
    }
    furi_string_free(temp_str);
}