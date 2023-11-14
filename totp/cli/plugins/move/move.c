#include <flipper_application/flipper_application.h>
#include <lib/toolbox/args.h>
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"
#include "../../cli_plugin_interface.h"
#include "../../../types/token_info.h"
#include "../../../services/config/config.h"
#include "../../../ui/scene_director.h"

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    int token_number;
    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);
    size_t total_count = totp_token_info_iterator_get_total_count(iterator_context);
    if(!args_read_int_and_trim(args, &token_number) || token_number < 1 ||
       (size_t)token_number > total_count) {
        TOTP_CLI_PRINT_INVALID_ARGUMENTS();
        return;
    }

    int new_token_number = 0;

    if(!args_read_int_and_trim(args, &new_token_number) || new_token_number < 1 ||
       (size_t)new_token_number > total_count) {
        TOTP_CLI_PRINT_INVALID_ARGUMENTS();
        return;
    }

    if(token_number == new_token_number) {
        TOTP_CLI_PRINTF_ERROR("New token number matches current token number\r\n");
        return;
    }

    TOTP_CLI_LOCK_UI(plugin_state);

    size_t token_index = token_number - 1;
    size_t new_token_index = new_token_number - 1;

    size_t original_token_index =
        totp_token_info_iterator_get_current_token_index(iterator_context);

    TOTP_CLI_PRINT_PROCESSING();

    if(totp_token_info_iterator_go_to(iterator_context, token_index) &&
       totp_token_info_iterator_move_current_token_info(iterator_context, new_token_index)) {
        TOTP_CLI_DELETE_LAST_LINE();
        TOTP_CLI_PRINTF_SUCCESS(
            "Token \"%s\" has been successfully updated\r\n",
            furi_string_get_cstr(
                totp_token_info_iterator_get_current_token(iterator_context)->name));
    } else {
        TOTP_CLI_DELETE_LAST_LINE();
        TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE();
    }

    totp_token_info_iterator_go_to(iterator_context, original_token_index);

    TOTP_CLI_UNLOCK_UI(plugin_state);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Move", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_move_plugin_ep() {
    return &plugin_descriptor;
}