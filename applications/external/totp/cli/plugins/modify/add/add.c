#include <flipper_application/flipper_application.h>
#include <lib/toolbox/args.h>
#include "../../../cli_helpers.h"
#include "../../../cli_shared_methods.h"
#include "../../../cli_plugin_interface.h"
#include "../../../../types/token_info.h"
#include "../../../../services/config/config.h"
#include "../../../../services/convert/convert.h"
#include "../../../../ui/scene_director.h"
#include "../common.h"

struct TotpAddContext {
    FuriString* args;
    Cli* cli;
    const CryptoSettings* crypto_settings;
};

enum TotpIteratorUpdateTokenResultsEx {
    TotpIteratorUpdateTokenResultInvalidSecret = 1,
    TotpIteratorUpdateTokenResultCancelled = 2,
    TotpIteratorUpdateTokenResultInvalidArguments = 3
};

static TotpIteratorUpdateTokenResult
    add_token_handler(TokenInfo* token_info, const void* context) {
    const struct TotpAddContext* context_t = context;

    // Reading token name
    if(!args_read_probably_quoted_string_and_trim(context_t->args, token_info->name)) {
        return TotpIteratorUpdateTokenResultInvalidArguments;
    }

    FuriString* temp_str = furi_string_alloc();

    // Read optional arguments
    bool mask_user_input = true;
    PlainTokenSecretEncoding token_secret_encoding = PlainTokenSecretEncodingBase32;
    while(args_read_string_and_trim(context_t->args, temp_str)) {
        bool parsed = false;
        if(!totp_cli_try_read_algo(token_info, temp_str, context_t->args, &parsed) &&
           !totp_cli_try_read_digits(token_info, temp_str, context_t->args, &parsed) &&
           !totp_cli_try_read_duration(token_info, temp_str, context_t->args, &parsed) &&
           !totp_cli_try_read_unsecure_flag(temp_str, &parsed, &mask_user_input) &&
           !totp_cli_try_read_automation_features(token_info, temp_str, context_t->args, &parsed) &&
           !totp_cli_try_read_plain_token_secret_encoding(
               temp_str, context_t->args, &parsed, &token_secret_encoding) &&
           !totp_cli_try_read_token_type(token_info, temp_str, context_t->args, &parsed) &&
           !totp_cli_try_read_token_counter(token_info, temp_str, context_t->args, &parsed)) {
            totp_cli_printf_unknown_argument(temp_str);
        }

        if(!parsed) {
            furi_string_free(temp_str);
            return TotpIteratorUpdateTokenResultInvalidArguments;
        }
    }

    // Reading token secret
    furi_string_reset(temp_str);
    TOTP_CLI_PRINTF("Enter token secret and confirm with [ENTER]:\r\n");
    if(!totp_cli_read_line(context_t->cli, temp_str, mask_user_input)) {
        TOTP_CLI_DELETE_LAST_LINE();
        furi_string_secure_free(temp_str);
        return TotpIteratorUpdateTokenResultCancelled;
    }

    TOTP_CLI_DELETE_LAST_LINE();

    bool secret_set = token_info_set_secret(
        token_info,
        furi_string_get_cstr(temp_str),
        furi_string_size(temp_str),
        token_secret_encoding,
        context_t->crypto_settings);

    furi_string_secure_free(temp_str);

    if(!secret_set) {
        return TotpIteratorUpdateTokenResultInvalidSecret;
    }

    return TotpIteratorUpdateTokenResultSuccess;
}

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);

    TOTP_CLI_LOCK_UI(plugin_state);

    struct TotpAddContext add_context = {
        .args = args, .cli = cli, .crypto_settings = &plugin_state->crypto_settings};
    TotpIteratorUpdateTokenResult add_result =
        totp_token_info_iterator_add_new_token(iterator_context, &add_token_handler, &add_context);

    if(add_result == TotpIteratorUpdateTokenResultSuccess) {
        TOTP_CLI_PRINTF_SUCCESS(
            "Token \"%s\" has been successfully added\r\n",
            furi_string_get_cstr(
                totp_token_info_iterator_get_current_token(iterator_context)->name));
    } else if(add_result == TotpIteratorUpdateTokenResultCancelled) {
        TOTP_CLI_PRINTF_INFO("Cancelled by user\r\n");
    } else if(add_result == TotpIteratorUpdateTokenResultInvalidArguments) {
        TOTP_CLI_PRINT_INVALID_ARGUMENTS();
    } else if(add_result == TotpIteratorUpdateTokenResultInvalidSecret) {
        TOTP_CLI_PRINTF_ERROR("Token secret seems to be invalid and can not be parsed\r\n");
    } else if(add_result == TotpIteratorUpdateTokenResultFileUpdateFailed) {
        TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE();
    }

    TOTP_CLI_UNLOCK_UI(plugin_state);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Add", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_add_plugin_ep() {
    return &plugin_descriptor;
}
