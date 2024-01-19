#include "cli.h"
#include <lib/toolbox/args.h>
#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/composite_resolver.h>
#include <loader/firmware_api/firmware_api.h>
#include "cli_helpers.h"
#include "plugins/timezone/meta.h"
#include "plugins/list/meta.h"
#include "plugins/modify/add/meta.h"
#include "plugins/modify/update/meta.h"
#include "plugins/delete/meta.h"
#include "plugins/help/meta.h"
#include "plugins/move/meta.h"
#include "plugins/pin/meta.h"
#include "plugins/notification/meta.h"
#include "plugins/reset/meta.h"
#include "plugins/automation/meta.h"
#include "plugins/details/meta.h"
#include "plugins/version/meta.h"
#include "plugins/export/meta.h"
#include "cli_plugin_interface.h"
#include "../app_api_interface.h"

struct TotpCliContext {
    PluginState* plugin_state;
    CompositeApiResolver* plugin_api_resolver;
};

static void totp_cli_print_unknown_command(const FuriString* unknown_command) {
    TOTP_CLI_PRINTF_ERROR(
        "Command \"%s\" is unknown. Use \"" TOTP_CLI_COMMAND_HELP
        "\" command to get list of available commands.",
        furi_string_get_cstr(unknown_command));
}

static void run_external_cli_plugin_handler(
    const char* handler_name,
    TotpCliContext* cli_context,
    FuriString* args,
    Cli* cli) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperApplication* plugin_app = flipper_application_alloc(
        storage, composite_api_resolver_get(cli_context->plugin_api_resolver));
    do {
        FuriString* full_handler_path =
            furi_string_alloc_printf(EXT_PATH("apps_data/totp/plugins/%s.fal"), handler_name);
        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(plugin_app, furi_string_get_cstr(full_handler_path));
        furi_string_free(full_handler_path);

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            TOTP_CLI_PRINTF_ERROR("Failed to preload plugin. Code: %d\r\n", preload_res);
            break;
        }

        if(!flipper_application_is_plugin(plugin_app)) {
            TOTP_CLI_PRINTF_ERROR("Plugin file is not a library\r\n");
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(plugin_app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            TOTP_CLI_PRINTF_ERROR("Failed to load plugin file. Code %d\r\n", load_status);
            break;
        }

        const FlipperAppPluginDescriptor* app_descriptor =
            flipper_application_plugin_get_descriptor(plugin_app);

        if(strcmp(app_descriptor->appid, PLUGIN_APP_ID) != 0) {
            TOTP_CLI_PRINTF_ERROR("Plugin doesn't seems to be a valid TOTP CLI plugin\r\n");
            break;
        }

        if(app_descriptor->ep_api_version != PLUGIN_API_VERSION) {
            TOTP_CLI_PRINTF_ERROR(
                "Plugin version %" PRIu32 " is not compatible with your app version\r\n",
                app_descriptor->ep_api_version);
            break;
        }

        const CliPlugin* plugin = app_descriptor->entry_point;

        plugin->handle(cli_context->plugin_state, args, cli);
    } while(false);
    flipper_application_free(plugin_app);

    furi_record_close(RECORD_STORAGE);
}

static void totp_cli_handler(Cli* cli, FuriString* args, void* context) {
    TotpCliContext* cli_context = context;

    FuriString* cmd = furi_string_alloc();

    args_read_string_and_trim(args, cmd);

    const char* external_plugin_name = NULL;

    if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_HELP) == 0 ||
       furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_HELP_ALT) == 0 ||
       furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_HELP_ALT2) == 0 || furi_string_empty(cmd)) {
        external_plugin_name = TOTP_CLI_PLUGIN_HELP_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_ADD) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_ADD_ALT) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_ADD_ALT2) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_ADD_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_LIST) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_LIST_ALT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_LIST_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_DELETE) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_DELETE_ALT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_DELETE_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_TIMEZONE) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_TIMEZONE_ALT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_TIMEZONE_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_MOVE) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_MOVE_ALT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_MOVE_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_PIN) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_PIN_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_NOTIFICATION) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_NOTIFICATION_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_AUTOMATION) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_AUTOMATION_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_RESET) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_RESET_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_UPDATE) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_UPDATE_FILE_NAME;
    } else if(
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_DETAILS) == 0 ||
        furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_DETAILS_ALT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_DETAILS_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_VERSION) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_VERSION_FILE_NAME;
    } else if(furi_string_cmp_str(cmd, TOTP_CLI_COMMAND_EXPORT) == 0) {
        external_plugin_name = TOTP_CLI_PLUGIN_EXPORT_FILE_NAME;
    } else {
        totp_cli_print_unknown_command(cmd);
    }

    if(external_plugin_name != NULL) {
        run_external_cli_plugin_handler(external_plugin_name, cli_context, args, cli);
    }

    furi_string_free(cmd);
}

TotpCliContext* totp_cli_register_command_handler(PluginState* plugin_state) {
    Cli* cli = furi_record_open(RECORD_CLI);
    TotpCliContext* context = malloc(sizeof(TotpCliContext));
    furi_check(context != NULL);
    context->plugin_state = plugin_state;

    context->plugin_api_resolver = composite_api_resolver_alloc();
    composite_api_resolver_add(context->plugin_api_resolver, firmware_api_interface);
    composite_api_resolver_add(context->plugin_api_resolver, application_api_interface);

    cli_add_command(
        cli, TOTP_CLI_COMMAND_NAME, CliCommandFlagParallelSafe, totp_cli_handler, context);
    furi_record_close(RECORD_CLI);
    return context;
}

void totp_cli_unregister_command_handler(TotpCliContext* context) {
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_delete_command(cli, TOTP_CLI_COMMAND_NAME);

    composite_api_resolver_free(context->plugin_api_resolver);

    furi_record_close(RECORD_CLI);
    free(context);
}
