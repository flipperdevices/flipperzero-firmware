#include <flipper_application/flipper_application.h>
#include <lib/toolbox/args.h>
#include "../../../services/config/config.h"
#include "../../../ui/scene_director.h"
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"
#include "../../cli_plugin_interface.h"

#define TOTP_CLI_COMMAND_NOTIFICATION_METHOD_NONE "none"
#define TOTP_CLI_COMMAND_NOTIFICATION_METHOD_SOUND "sound"
#define TOTP_CLI_COMMAND_NOTIFICATION_METHOD_VIBRO "vibro"

static void
    totp_cli_command_notification_print_method(NotificationMethod method, const char* color) {
    bool has_previous_method = false;
    if(method & NotificationMethodSound) {
        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_NOTIFICATION_METHOD_SOUND "\"");
        has_previous_method = true;
    }
    if(method & NotificationMethodVibro) {
        if(has_previous_method) {
            TOTP_CLI_PRINTF_COLORFUL(color, " and ");
        }

        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_NOTIFICATION_METHOD_VIBRO "\"");
    }
    if(method == NotificationMethodNone) {
        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_NOTIFICATION_METHOD_NONE "\"");
    }
}

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    FuriString* temp_str = furi_string_alloc();
    bool new_method_provided = false;
    NotificationMethod new_method = NotificationMethodNone;
    bool args_valid = true;
    while(args_read_string_and_trim(args, temp_str)) {
        if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_NOTIFICATION_METHOD_NONE) == 0) {
            new_method_provided = true;
            new_method = NotificationMethodNone;
        } else if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_NOTIFICATION_METHOD_SOUND) == 0) {
            new_method_provided = true;
            new_method |= NotificationMethodSound;
        } else if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_NOTIFICATION_METHOD_VIBRO) == 0) {
            new_method_provided = true;
            new_method |= NotificationMethodVibro;
        } else {
            args_valid = false;
            break;
        }
    }

    do {
        if(!args_valid) {
            TOTP_CLI_PRINT_INVALID_ARGUMENTS();
            break;
        }

        if(new_method_provided) {
            TOTP_CLI_LOCK_UI(plugin_state);

            plugin_state->notification_method = new_method;
            if(totp_config_file_update_notification_method(plugin_state)) {
                TOTP_CLI_PRINTF_SUCCESS("Notification method is set to ");
                totp_cli_command_notification_print_method(new_method, TOTP_CLI_COLOR_SUCCESS);
                cli_nl();
            } else {
                TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE();
            }

            TOTP_CLI_UNLOCK_UI(plugin_state);
        } else {
            TOTP_CLI_PRINTF_INFO("Current notification method is ");
            totp_cli_command_notification_print_method(
                plugin_state->notification_method, TOTP_CLI_COLOR_INFO);
            cli_nl();
        }
    } while(false);

    furi_string_free(temp_str);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Notification", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_notification_plugin_ep() {
    return &plugin_descriptor;
}