#include <flipper_application/flipper_application.h>
#include <lib/toolbox/args.h>
#include "../../../services/config/config.h"
#include "../../../services/config/constants.h"
#include "../../../types/token_info.h"
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"
#include "../../cli_plugin_interface.h"
#include "../../../ui/scene_director.h"
#include "formatters/table/details_output_formatter_table.h"
#include "formatters/tsv/details_output_formatter_tsv.h"

typedef void (*TOTP_CLI_DETAILS_HEADER_FORMATTER)();
typedef void (*TOTP_CLI_DETAILS_FOOTER_FORMATTER)();
typedef void (*TOTP_CLI_DETAILS_AUTOMATION_FEATURE_ITEM_FORMATTER)(
    const char* key,
    const char* feature,
    bool* header_printed);
typedef void (*TOTP_CLI_DETAILS_CSTR_FORMATTER)(const char* key, const char* value);
typedef void (*TOTP_CLI_DETAILS_UINT8T_FORMATTER)(const char* key, uint8_t value);
typedef void (*TOTP_CLI_DETAILS_SIZET_FORMATTER)(const char* key, size_t value);
typedef void (*TOTP_CLI_DETAILS_UINT64T_FORMATTER)(const char* key, uint64_t value);

typedef struct {
    const TOTP_CLI_DETAILS_HEADER_FORMATTER header_formatter;
    const TOTP_CLI_DETAILS_FOOTER_FORMATTER footer_formatter;
    const TOTP_CLI_DETAILS_AUTOMATION_FEATURE_ITEM_FORMATTER automation_feature_item_formatter;
    const TOTP_CLI_DETAILS_CSTR_FORMATTER cstr_formatter;
    const TOTP_CLI_DETAILS_UINT8T_FORMATTER uint8t_formatter;
    const TOTP_CLI_DETAILS_SIZET_FORMATTER sizet_formatter;
    const TOTP_CLI_DETAILS_UINT64T_FORMATTER uint64t_formatter;
} TotpCliDetailsFormatter;

static const TotpCliDetailsFormatter available_formatters[] = {
    {.header_formatter = &details_output_formatter_print_header_table,
     .footer_formatter = &details_output_formatter_print_footer_table,
     .automation_feature_item_formatter = &details_output_formatter_print_automation_feature_table,
     .cstr_formatter = &details_output_formatter_print_cstr_table,
     .uint8t_formatter = &details_output_formatter_print_uint8t_table,
     .sizet_formatter = &details_output_formatter_print_sizet_table,
     .uint64t_formatter = &details_output_formatter_print_uint64t_table},

    {.header_formatter = &details_output_formatter_print_header_tsv,
     .footer_formatter = &details_output_formatter_print_footer_tsv,
     .automation_feature_item_formatter = &details_output_formatter_print_automation_feature_tsv,
     .cstr_formatter = &details_output_formatter_print_cstr_tsv,
     .uint8t_formatter = &details_output_formatter_print_uint8t_tsv,
     .sizet_formatter = &details_output_formatter_print_sizet_tsv,
     .uint64t_formatter = &details_output_formatter_print_uint64t_tsv},
};

static void print_automation_features(
    const TokenInfo* token_info,
    const TotpCliDetailsFormatter* formatter) {
    bool header_printed = false;
    const char* AUTOMATION_FEATURES_PRINT_KEY = "Automation features";
    if(token_info->automation_features == TokenAutomationFeatureNone) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "None", &header_printed);
        return;
    }

    if(token_info->automation_features & TokenAutomationFeatureEnterAtTheEnd) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type <Enter> key at the end", &header_printed);
    }

    if(token_info->automation_features & TokenAutomationFeatureTabAtTheEnd) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type <Tab> key at the end", &header_printed);
    }

    if(token_info->automation_features & TokenAutomationFeatureTypeSlower) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type slower", &header_printed);
    }
}

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    int token_number;
    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);
    if(!args_read_int_and_trim(args, &token_number) || token_number <= 0 ||
       (size_t)token_number > totp_token_info_iterator_get_total_count(iterator_context)) {
        TOTP_CLI_PRINT_INVALID_ARGUMENTS();
        return;
    }

    const TotpCliDetailsFormatter* formatter = &available_formatters[0];
    FuriString* arg = furi_string_alloc();
    if(args_read_string_and_trim(args, arg) && furi_string_cmpi_str(arg, "--tsv") == 0) {
        formatter = &available_formatters[1];
    }

    furi_string_free(arg);

    TOTP_CLI_LOCK_UI(plugin_state);

    size_t original_token_index =
        totp_token_info_iterator_get_current_token_index(iterator_context);
    if(totp_token_info_iterator_go_to(iterator_context, token_number - 1)) {
        const TokenInfo* token_info = totp_token_info_iterator_get_current_token(iterator_context);

        (*formatter->header_formatter)();
        (*formatter->sizet_formatter)("Index", token_number);
        (*formatter->cstr_formatter)("Type", token_info_get_type_as_cstr(token_info));
        (*formatter->cstr_formatter)("Name", furi_string_get_cstr(token_info->name));
        (*formatter->cstr_formatter)("Hashing algorithm", token_info_get_algo_as_cstr(token_info));
        (*formatter->uint8t_formatter)("Number of digits", token_info->digits);
        if(token_info->type == TokenTypeTOTP) {
            (*formatter->uint8t_formatter)("Token lifetime", token_info->duration);
        } else if(token_info->type == TokenTypeHOTP) {
            (*formatter->uint64t_formatter)("Token counter", token_info->counter);
        }
        print_automation_features(token_info, formatter);
        (*formatter->footer_formatter)();
    } else {
        TOTP_CLI_PRINT_ERROR_LOADING_TOKEN_INFO();
    }

    totp_token_info_iterator_go_to(iterator_context, original_token_index);

    TOTP_CLI_UNLOCK_UI(plugin_state);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Details", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_details_plugin_ep() {
    return &plugin_descriptor;
}
